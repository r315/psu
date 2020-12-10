
#include <console.h>

#include "FreeRTOS.h"
#include "task.h"

#include "psu.h"

#include "misc.h"
#include "cmdhelp.h"
#include "cmdadc.h"
#include "cmdpwm.h"
#include "cmddfu.h"
#include "cmdset.h"
#include "cmdstatus.h"
#include "cmdeeprom.h"

#include "bui.h"
#include "model_psu.h"
#include "presenter_psu.h"
#include "presenter_preset.h"
#include "presenter_charger.h"
#include "presenter_load.h"

static psu_t psu;

static Console console;
static CmdHelp help;
static CmdAdc adc1;
static CmdPwm pwm;
static CmdPwr pwr;
static CmdIo io;
static CmdSet set;
static CmdReset reset;
#ifdef ENABLE_EEPROM
static CmdEeprom eeprom;
#endif
#ifdef ENABLE_DFU
static CmdDfu dfu;
#endif
static CmdStatus status;

extern const uint8_t icon_out[];
extern const uint8_t icon_psu[];
extern const uint8_t icon_load[];
extern const uint8_t icon_chr[];
extern const uint8_t dro_unit_v[];
extern const uint8_t dro_unit_a[];


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
#ifdef ENABLE_EEPROM
    &eeprom,
#endif
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
    5.7f,  // MUX8  B+
    4.3f,  // MUX9  VB3
    3.0f,  // MUX10 VB2
    2.0f,  // MUX11 VB1
    1.0f,  // MUX12 B-
    1.0f,  // MUX13 IUSB
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
void psu_poweroff(void){
    // TODO: Properly disable all peripherals (clock and pwr) and then power off
    psu_setOutputEnable(FALSE);
    psu_setLoadCurrent(0);
    LCD_Bkl(OFF);
    vTaskDelay(pdMS_TO_TICKS(POWER_OFF_DELAY));
    SOFT_POWER_OFF;
}

void psu_setOutputEnable(uint8_t en){
    PSU_OE_SET(en);
}

void app_setOutputEnable(uint8_t en){
    if(en){
        SET_OE_FLAG;
    }else{
        CLR_OE_FLAG;
    }
    psu_setOutputEnable(en);
}

uint8_t app_toggleOutputEnable(void){
    app_setOutputEnable(!GET_OE_FLAG);
    return GET_OE_FLAG;
}

void app_setLoadEnable(uint8_t en){
    if(en){
        SET_LD_FLAG;
    }else{
        CLR_LD_FLAG;
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

/**
 * @brief Get cell voltage at balancer terminals
 * \param c : cell number [0,3], counting from gnd
 * \return : cell voltage in mV
 * */
uint32_t psu_getCellVoltage(uint8_t c){
    return psu_getChannelVoltage(VB0_MUX_CH - (c + 1));
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
void app_processPowerButton(void){
    static uint16_t pwr_off_counter = POWER_OFF_COUNT;

    //DBG_PRINT("ADC2 :%x\n", ADC2_Read(ADC_PWR_SW_CH));

    if(GET_PWR_BTN){
        if(--pwr_off_counter == 0){
            DBG_PRINT("Powering off...\n");
            //app_saveState();
            psu_poweroff();
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

preset_t app_getCurrentPreset(void){
    return psu.preset_list[psu.preset_idx];
}

preset_t app_getPreset(uint8_t idx){
    return psu.preset_list[idx];
}

uint8_t app_getCurrentPresetIdx(void){
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

void app_setPresetByIdx(uint8_t idx){
    psu.preset_idx = idx;
    app_applyPreset();
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
#ifdef ENABLE_EEPROM
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
#else
    app_defaultState();
#endif
    return 1;
}

uint8_t app_saveState(void){
#ifdef ENABLE_EEPROM
    uint16_t size;
    size = (uint8_t*)&psu.cksum - (uint8_t*)&psu;
    psu.cksum = app_calcCksum((uint8_t*)&psu, size);
    DBG_PRINT("Writting %u bytes to eeprom, cksum 0x%2X\n", size, psu.cksum);
    if(!EEPROM_Write(EEPROM_APP_OFFSET, (uint8_t*)&psu, size + 1)){
        return 0;
    }
    DBG_PRINT("done\n");
#endif
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

    BUIPresenter *presenter_psu = new PresenterPsu();
    BUIPresenter *presenter_preset = new PresenterPreset();
    BUIPresenter *presenter_charger = new PresenterCharger();
    BUIPresenter *presenter_load = new PresenterLoad();

    BUI bui(model_psu);
   
    bui.addPresenter(presenter_psu);
    bui.addPresenter(presenter_preset);
    bui.addPresenter(presenter_charger);
    bui.addPresenter(presenter_load);
    
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

    model_psu.init();

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
#ifdef ENABLE_EEPROM
    EEPROM_Init();
#endif
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
    DBG_PRINT("Memory allocation fail\n");
}

void *operator new(size_t size){
    DBG_PRINT("Allocating: %u bytes\n", size);
    return pvPortMalloc(size);
}

void operator delete(void *ptr){
    vPortFree(ptr);
    DBG_PRINT("Free mem: %u bytes\n", xPortGetFreeHeapSize());
}