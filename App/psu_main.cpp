
#include "psu.h"
#include "draw.h"

#include <console.h>
#include "cmdhelp.h"
#include "cmdadc.h"
#include "cmdpwm.h"
#include "cmddfu.h"
#include "cmdpwr.h"
#include "cmdio.h"
#include "cmdset.h"
#include "cmdstatus.h"
#include "graph.h"
#include "cmdeeprom.h"

char gOut[10];
static psu_t psu;
static ScreenPsu cpsu;
static ScreenLoad cload;
static ScreenCharger ccharger;
static ScreenPreset cpreset;
static preset_t preset_list[MAX_PRESETS];
static pwmcal_t pwmcal[PWM_NUM_CH];
static float an_channel_gain[AN_MUX_NUM_CH]; 

static Screen *modes[] = {
    &cpsu,
    &cpreset,
    &cload,
    //&ccharger,
};

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
    {490, (1<<PWM_RESOLUTION), 1020},    // pwm1 calibration <min, max, start>
    {0, (1<<PWM_RESOLUTION), 100},    // pwm2 calibration
    {0, (1<<PWM_RESOLUTION), 100},    // pwm3 calibration
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
    1.0f,  // I1
    1.0f,  // V2
    1.0f,  // I2
    1.0f,  // V3
};

/**
 * PSU
 * */

extern "C" void psu_adc_cb(uint16_t *data){
    psu.adc_data = data;
    SET_AD_FLAG;
}

static void mapAndSetPwm(float x, float in_max, float in_min, uint8_t ch){
    uint16_t pwm_value = (x - in_min) * (psu.pwm_cal[ch].max - psu.pwm_cal[ch].min) / (in_max - in_min) + psu.pwm_cal[ch].min;
    PWM_Set(ch, pwm_value);
}

/**
 * PSU public control functions
 * */
void psu_setOutputVoltage(uint32_t mv){    
    mapAndSetPwm(mv, MIN_VOLTAGE, MAX_VOLTAGE, PWM_CH_VOLTAGE);
}

void psu_setOutputCurrent(uint32_t ma){
    mapAndSetPwm(ma, MAX_CURRENT, MIN_CURRENT, PWM_CH_CURRENT);
}

void psu_setInputLoad(uint32_t ma){
    mapAndSetPwm(ma, MAX_LOAD, MIN_LOAD, PWM_CH_LOAD);
}

void psu_setOutputEnable(uint8_t en){    
    if(en){
        SET_OE_FLAG;
        DRAW_Icon(OUTPUT_ICON_POS, (uint8_t *)icon_out, RED);
    }else{
        CLR_OE_FLAG;
        DRAW_FillRect(OUTPUT_ICON_POS, icon_out[0], icon_out[1], BLACK);        
    }

    PSU_OE_SET(en);
}

void psu_setLoadEnable(uint8_t en){
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
    return psu.adc_data[channel] * ADC_GetResolution() * an_channel_gain[channel];
}
uint32_t psu_getVoltage(void){
    return psu_getChannelVoltage(VOUT_MUX_CH);
}

uint32_t psu_getCurrent(void){
    return psu_getChannelVoltage(IOUT_MUX_CH);
}

uint8_t psu_getOutputEnable(void){
    return  GET_OE_FLAG;
}

uint8_t psu_getLoadEnabled(void){
    return GET_LD_FLAG;
}

uint8_t psu_AdcReady(void){
    return GET_AD_FLAG;
}

void psu_setLoadCurrent(uint32_t ma){

}

uint32_t psu_getLoadCurrent(void){
    return 0;
}

/**
 * Application api
 * */
preset_t *app_getPreset(void){
    return psu.preset;
}

preset_t *app_getPresetList(void){
    return (preset_t*)preset_list;
}

void app_setPreset(preset_t *pre){
    psu.preset = pre;
    psu_setOutputVoltage(psu.preset->v);
    psu_setOutputCurrent(psu.preset->i);
}

void app_selectMode(uint8_t mode){
    
    if(mode >= MAX_MODES){
        return;
    }
    psu.cur_mode = mode;    
    (modes[mode])->init();
    // Mode clears screen, so must redraw output icon,
    // only if active
    psu_setOutputEnable(GET_OE_FLAG);
    psu_setLoadEnable(GET_LD_FLAG);
}

void app_cycleMode(void){
uint8_t mode = psu.cur_mode + 1;
    
    if(mode == MAX_MODES ){
        mode = 0;
    }
    
    app_selectMode(mode);
}

/**
 * @brief Read buttons connected to I2C io expander
 * */
void app_checkButtons(){
    BUTTON_Read();
    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        switch(BUTTON_VALUE){
            case BUTTON_MODE: app_cycleMode(); break;
            case BUTTON_OUT: psu_setOutputEnable(!GET_OE_FLAG); break;
        }
    }
}

/**
 * @brief
 * 
 * */
void app_InitEEPROM(void){

    memcpy(pwmcal, default_pwm_calibration, sizeof(default_pwm_calibration));
    memcpy(preset_list, default_preset, sizeof(default_preset));
    memcpy(an_channel_gain, default_an_channel_gain, sizeof(default_an_channel_gain));

    psu.pwm_cal = pwmcal;
    psu.preset = preset_list;

/*
uint8_t bind_flag;
    
    if(EEPROM_Read(EEPROM_BIND_FLAG, &bind_flag, 1) != 1){
        DBG_PRINT("Error reading EEPROM\n");
        return;
    }
        
    if(bind_flag == BIND_FLAG_VALUE){    
        if(NV_Restore(dst, EEPROM_SIZE) != EEPROM_SIZE){
            DBG_PRINT("Error reading EEPROM\n");
            return;
        }
        DBG_PRINT("Data loaded from EEPROM\n");
    }
*/
}

/**
 * @brief Save the ram eeprom content to flash memory
 * Note: In order to save eeprom
 * 
 * */
void app_SaveEEPROM(void){

   /*  // FIXME: Really should implement CRC
    *((uint8_t*)eeprom_data+EEPROM_BIND_FLAG) = BIND_FLAG_VALUE;

    EEPROM_Write(EEPROM_ID_OFFSET, (uint8_t*)eeprom_data, EEPROM_SIZE);

    if(!EEPROM_Sync()){
        DBG_PRINT("!! Fail to sync EEPROM !!\n");
    }else{
        DBG_PRINT("EEPROM Saved\n");
    } */
}

#ifndef USE_ADCMGR
/**
 * ADC End of convertion callback
 * */
static void app_adcEocCb(uint16_t *res){
uint32_t *src = (uint32_t*)res;
    for (uint32_t i = 0; i < ADC_SEQ_LEN; i++, src++)
    {
        psu.adcvalues[i] = *src;
    }
}
#endif

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
 * Called every 10ms by Timer4 interrupt, as console
 * may block the main thread, having a secondary loop
 * ensures operation of lcd update and button handling
 * */
void tskPsu(void *ptr){
static TickType_t xLastWakeTime;
uint8_t count = 0;

    app_selectMode(psu.cur_mode);

    ADCMGR_Start();

    SET_ADCMGR_FLAG;

    LCD_Bkl(TRUE);

    while(1){
        app_checkButtons();
        //DBG_PIN_HIGH;
        modes[psu.cur_mode]->process();
        //DBG_PIN_LOW;        
        if(GET_AD_FLAG && GET_ADCMGR_FLAG){
            CLR_AD_FLAG;
            ADCMGR_Start();
        }

        if(((++count)&0x0f) == 0){
            LED_ON;
        }else{
            LED_OFF;
        }

        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(UPDATE_INTERVAL));
    }
}

void tskCmdLine(void *ptr){
    
    stdout_t *stdio_port = (stdout_t*)ptr;    

    console.init(stdio_port, CONSOLE_PROMPT);
    console.registerCommandList(commands);

    // wait for usb to start if connected
    vTaskDelay(pdMS_TO_TICKS(500));

    console.cls();

    while(1){
        console.process();
    }
}

extern "C" void app_setup(void){  
    BOARD_Init();

    stdio_ops.init();

    #ifdef ENABLE_DEBUG
    dbg_init(&stdio_ops);
    #endif

    psu_setOutputEnable(FALSE);

    EXPANDER_Init();

    LCD_Init();
    LCD_Rotation(LCD_LANDSCAPE);

    TEXT_Init();

    EEPROM_Init();

    app_InitEEPROM();

    PWM_Init();

    ADCMGR_Init();

    ADCMGR_SetSequence(NULL,0 , psu_adc_cb);

    psu_setOutputVoltage(psu.preset->v);

    psu_setOutputCurrent(psu.preset->i);

    xTaskCreate( tskCmdLine, "CLI", configMINIMAL_STACK_SIZE * 4, &stdio_ops, PRIORITY_LOW, NULL );
    xTaskCreate( tskPsu, "PSU", configMINIMAL_STACK_SIZE * 4, NULL, PRIORITY_LOW + 1, NULL );
}


