
#include <console.h>

#include "FreeRTOS.h"
#include "task.h"

#include "psu.h"
#include "draw.h"
#include "graph.h"
#include "text.h"

#include "misc.h"
#include "cmdhelp.h"
#include "cmdadc.h"
#include "cmdpwm.h"
#include "cmddfu.h"
#include "cmdset.h"
#include "cmdstatus.h"
#include "cmdeeprom.h"

#include "bui.h"
#include "view_psu.h"
#include "model_psu.h"
#include "presenter_psu.h"

static psu_t psu;

static Console console;
static CmdHelp help;
static CmdAdc adc1;
static CmdPwm pwm;
static CmdPwr pwr;
static CmdIo io;
static CmdSet set;
static CmdReset reset;
static CmdEeprom eeprom;
#ifdef ENABLE_DFU
static CmdDfu dfu;
#endif
static CmdStatus status;

static ConsoleCommand *commands[] = {
    &help,
    &adc1,
    &pwm,
    &pwr,
    &io,
    &set,
#ifdef ENABLE_DFU
    &dfu,
#endif
    &reset,
    &status,
    &eeprom,
    NULL
};

const pwmcal_t default_pwm_calibration[] = {
    {490, (1<<PWM_RESOLUTION), 1020},   // pwm1 (VOUT) calibration <min, max, start>
    {0, (1<<PWM_RESOLUTION), 100},      // pwm2 (IOUT) calibration
    {0, (1<<PWM_RESOLUTION), 0},        // pwm3 (ILOAD) calibration MUST be 0
};

const preset_t default_preset[] = {
    {1200, 100},   // <mv, ma>
    {1800, 100},
    {2500, 100},
    {3300, 1600},
    {5000, 300},
    {9600, 50}
};

const float default_an_channel_gain[] = {
    6.1f,  // V1  gain = 1/(R300/(R300+R301))
    0.9f,  // I1
    6.1f,  // V2  gain = 1/(R402/(R402+R403))
    1.0f,  // I2
    6.1f,  // V3  gain = 1/(R205/(R205+R203))
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,  // IUSB
    1.0f,
    1.0f,
    1.0f
};

/**
 * PSU
 * */

/**
 * @brief Callback from adc manager
 * 
 * \param data : pointer to raw adc data. The number of results is equal 
 * to the size of the conversion sequence, and only valid until the next call tp ADCMGR_Start()
 *
 * */
extern "C" void psu_adc_cb(uint16_t *data){
    psu.adc_data = data;
    SET_AD_FLAG;
}

static void mapAndSetPwm(float x, float in_max, float in_min, uint8_t ch){
    uint16_t pwm_value = (x - in_min) * (psu.pwm_cal[ch].max - psu.pwm_cal[ch].min) / (in_max - in_min) + psu.pwm_cal[ch].min;
    PWM_Set(ch, pwm_value);
    DBG_PRINT("Set pwm %d %u\n",ch, pwm_value);
}

static uint8_t app_calcCksum(uint8_t *src, uint16_t len){
    uint8_t sum = 0;

    while(len--){
        sum += *src++;
    }

    return 256 - sum;
}

/**
 * PSU public control functions
 * */
void psu_setOutputEnable(uint8_t en){
    PSU_OE_SET(en);
}

void app_setOutputEnable(uint8_t en){
    if(en){
        SET_OE_FLAG;
        DRAW_Icon(OUTPUT_ICON_POS, (uint8_t *)icon_out, RED);
    }else{
        CLR_OE_FLAG;
        DRAW_FillRect(OUTPUT_ICON_POS, icon_out[0], icon_out[1], BLACK);        
    }
    psu_setOutputEnable(en);
}

uint8_t app_toggleOutputEnable(void){
    app_setOutputEnable(!GET_OE_FLAG);
    return !GET_OE_FLAG;
}

void app_setLoadEnable(uint8_t en){
    if(en){
        SET_LD_FLAG;
        DRAW_Icon(LOAD_ICON_POS, (uint8_t *)icon_load, GREEN);
    }else{
        CLR_LD_FLAG;
        DRAW_FillRect(LOAD_ICON_POS, icon_load[0], icon_load[1], BLACK);        
    }
}

/**
 * @brief Get channel voltage
 * 
 * \param channel : Channel 
 * \return : voltage in mv
 * */
uint32_t psu_getChannelVoltage(uint8_t channel){
    return psu.adc_data[channel] * ADC_GetResolution() * psu.an_channel_gain[channel];
}

uint32_t psu_getOutputVoltage(void){
    return psu_getChannelVoltage(VOUT_MUX_CH);
}

void psu_setOutputVoltage(uint32_t mv){    
    mapAndSetPwm(mv, MIN_VOLTAGE, MAX_VOLTAGE, PWM_CH_VOLTAGE);
}

uint32_t psu_getOutputCurrent(void){
    return psu_getChannelVoltage(IOUT_MUX_CH);
}

void psu_setOutputCurrent(uint32_t ma){
    mapAndSetPwm(ma, MAX_CURRENT, MIN_CURRENT, PWM_CH_CURRENT);
}

uint32_t psu_getLoadCurrent(void){
    return psu_getChannelVoltage(ILOAD_MUX_CH);;
}

void psu_setLoadCurrent(uint32_t ma){
    mapAndSetPwm(ma, MAX_ILOAD, MIN_ILOAD, PWM_CH_LOAD);
}

uint32_t psu_getInputVoltage(void){
    return psu_getChannelVoltage(VIN_MUX_CH);
}

uint32_t psu_getLoadVoltage(void){
    return psu_getChannelVoltage(VLOAD_MUX_CH);
}

uint32_t psu_getUsbCurrent(void){
    return psu_getChannelVoltage(IUSB_MUX_CH);
}

/**
 * Application api
 * */
void app_poweroff(void){
    // TODO: Properly disable all peripherals (clock and pwr) and then power off
    psu_setOutputEnable(FALSE);
    psu_setLoadCurrent(0);
    app_saveState();
    LCD_Bkl(OFF);
    vTaskDelay(pdMS_TO_TICKS(POWER_OFF_DELAY));
    SOFT_POWER_OFF;
}

void app_processPowerButton(void){
    static uint16_t pwr_off_counter = POWER_OFF_COUNT;
    if(GET_PWR_BTN){
        if(--pwr_off_counter == 0){
            app_poweroff();
    }
    }else{
        pwr_off_counter = POWER_OFF_COUNT;
    }
}

uint8_t app_isLoadEnabled(void){
    return GET_LD_FLAG;
}

uint8_t app_isAdcDone(void){
    return GET_AD_FLAG;
}

uint8_t app_isOutputEnabled(void){
    return  GET_OE_FLAG;
}

preset_t app_getPreset(void){
    return psu.preset_list[psu.preset_idx];
}

uint8_t app_getPresetIdx(void){
    return psu.preset_idx;
}

preset_t *app_getPresetList(void){
    return psu.preset_list;
}

static void app_applyPreset(){
    psu_setOutputVoltage(psu.preset_list[psu.preset_idx].v);
    psu_setOutputCurrent(psu.preset_list[psu.preset_idx].i);
}

void app_setPreset(preset_t pre){
    memcpy(&psu.preset_list[psu.preset_idx], &pre, sizeof(preset_t));
    app_applyPreset();
}

void app_setPresetIdx(uint8_t idx){
    psu.preset_idx = idx;
    app_applyPreset();
}

void app_selectScreen(uint8_t screen_idx){
    
    if(screen_idx >= SCREEN_NUM){
        return;
    }
    psu.screen_idx = screen_idx;    
    //(screens[screen_idx])->init();
    // Mode clears screen, so must redraw output icon,
    // only if active
    app_setOutputEnable(GET_OE_FLAG);
    app_setLoadEnable(GET_LD_FLAG);
}

static void app_cycleMode(void){
    uint8_t screen_idx = psu.screen_idx + 1;
    
    if(screen_idx == SCREEN_NUM ){
        screen_idx = 0;
    }
    
    app_selectScreen(screen_idx);
}

/**
 * @brief Read buttons connected to I2C io expander
 * */
void app_checkButtons(){
    BUTTON_Read();
    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        switch(BUTTON_VALUE){
            case BUTTON_MODE: app_cycleMode(); break;
            case BUTTON_OUT: app_setOutputEnable(!GET_OE_FLAG); break;
        }
    }
}

/**
 * @brief Load default values in to EEPROM
 * */
void app_defaultState(void){
    memcpy(psu.pwm_cal, default_pwm_calibration, sizeof(default_pwm_calibration));
    memcpy(psu.preset_list, default_preset, sizeof(default_preset));
    memcpy(psu.an_channel_gain, default_an_channel_gain, sizeof(default_an_channel_gain));
    psu.preset_idx = 0;
    psu.screen_idx = 0;
    psu.flags = 0;
}

/**
 * @brief
 * 
 * */
uint8_t app_restoreState(void){

    uint16_t size = (uint8_t*)&psu.cksum - (uint8_t*)&psu;

    EEPROM_Read(EEPROM_APP_OFFSET, (uint8_t*)&psu, size + 1);
    
    uint8_t cksum = app_calcCksum((uint8_t*)&psu, size);
    if( psu.cksum != cksum){
        //DBG_PRINT("Invalid data on EEPROM\n");
        app_defaultState();
        CLR_EEPROM_FLAG;
        return 0;
    }
    SET_EEPROM_FLAG;
    return 1;
}

uint8_t app_saveState(void){
    uint16_t size;
    size = (uint8_t*)&psu.cksum - (uint8_t*)&psu;
    psu.cksum = app_calcCksum((uint8_t*)&psu, size);
    DBG_PRINT("Writting %u bytes to eeprom, cksum 0x%2X\n", size, psu.cksum);
    if(!EEPROM_Write(EEPROM_APP_OFFSET, (uint8_t*)&psu, size + 1)){
        return 0;
    }
    DBG_PRINT("done\n");
    return 1;
}

/**
 * @brief enables/disables adc manager
 * Use for testing, inibits/allow tsk_psu of calling ADCMGR_Start
 * */
void app_enable_adcmgr(uint8_t en){
    if(en){
        ADCMGR_SetSequence(NULL,0 , psu_adc_cb);
        SET_ADCMGR_FLAG;
    }
    else
        CLR_ADCMGR_FLAG;   
}
/**
 * Model
 * */
ModelPsu model_psu;

/**
 * 
 * */
void tskBui(void *ptr){
    static TickType_t xLastWakeTime;

    ViewPsu view_psu;
    PresenterPsu presenter_psu;
    BUI bui(model_psu);
   
    bui.createScreen((BUIView*)&view_psu, (BUIPresenter*)&presenter_psu);
    
    LCD_Bkl(TRUE);

    while(1){
        bui.handler(NULL);
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(UPDATE_INTERVAL));
    }
}

/**
 * 
 * */
void tskPsu(void *ptr){
static TickType_t xLastWakeTime;
uint8_t count = 0;   

    ADCMGR_Start();

    SET_ADCMGR_FLAG;

    model_psu.updateOutPreset(app_getPreset());

    while(1){
        //app_checkButtons();
        app_processPowerButton();
        //DBG_PIN_HIGH;
        
        //DBG_PIN_LOW;        
        if(GET_AD_FLAG && GET_ADCMGR_FLAG){
            //model_update((model_t*)&psu.adc_data);
            model_psu.update();
            CLR_AD_FLAG;
            ADCMGR_Start();
        }

        if(((++count)&0x0f) == 0){
            LED_ON;
        }else{
            LED_OFF;
        }
        
        #ifndef ENABLE_DEBUG
        reloadWatchDog();
        #endif
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(UPDATE_INTERVAL));
    }
}

void tskCmdLine(void *ptr){
    
    stdout_t *stdio_port = (stdout_t*)ptr;

    stdio_port->init();

    #ifdef ENABLE_DEBUG
    dbg_init(stdio_port);
    #endif

    console.init(stdio_port, CONSOLE_PROMPT);
    console.registerCommandList(commands);

    // wait for usb to start if connected
    //vTaskDelay(pdMS_TO_TICKS(500));

    console.cls();

    console.print("PSU v%u.%u.%u\n", PSU_VERSION_MAJOR, PSU_VERSION_MINOR, PSU_VERSIO_PATCH);
    console.print("FreeRTOS %s\n", tskKERNEL_VERSION_NUMBER);
    console.print("Free mem: %u bytes\n", xPortGetFreeHeapSize());
    console.print("\n");

    while(1){
        console.process();
    }
}

static void startTask(void(*task)(void*), const char *name, void *args, uint32_t stack, int priority){
    if(xTaskCreate( task, name, stack, args, priority, NULL ) != pdPASS){
        dbg_printf("FAIL: to start task %s\n", name);    
    }
}

extern "C" void app_setup(void){  
    BOARD_Init();  

    psu_setOutputEnable(FALSE);

    EXPANDER_Init();

    LCD_Init();
    LCD_Rotation(LCD_LANDSCAPE);

    TEXT_Init();

    EEPROM_Init();

    app_restoreState();

    PWM_Init(psu.pwm_cal[0].init, psu.pwm_cal[1].init, psu.pwm_cal[2].init);

    ADCMGR_Init();

    ADCMGR_SetSequence(NULL,0 , psu_adc_cb);

    // Configure watchdog
    #ifndef ENABLE_DEBUG
    enableWatchDog(WATCHDOG_TIME);
    #endif
    
    startTask(tskCmdLine, "CLI", &stdio_ops, configMINIMAL_STACK_SIZE * 2, PRIORITY_LOW);
    startTask(tskPsu, "PSU", NULL, configMINIMAL_STACK_SIZE, PRIORITY_LOW + 1);
    startTask(tskBui, "BUI", NULL, configMINIMAL_STACK_SIZE * 8, PRIORITY_LOW);
}

extern "C" void vApplicationMallocFailedHook( void ){
    dbg_printf("Memory allocation fail\n");
}

