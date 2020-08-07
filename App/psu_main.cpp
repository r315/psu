
#include "psu.h"
#include "draw.h"

char gOut[10];
static psu_t psu;
static ScreenPsu cpsu;
static ScreenLoad cload;
static ScreenCharger ccharger;
static ScreenPreset cpreset;

static Screen *modes[] = {
    &cpsu,
    &cpreset,
    &cload,
    &ccharger,
};

static Console console;
static CmdHelp help;
static CmdAdc adc1;
static CmdPwm pwm;
static CmdPwr pwr;
static CmdIo io;
static CmdSet set;
static CmdReset reset;
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
    NULL
};

pwmcal_t default_pwm_calibration[] = {
    {0, (1<<PWM_RESOLUTION), 100},    // pwm1 calibration <min, max, start>
    {0, (1<<PWM_RESOLUTION), 100},    // pwm2 calibration
    {0, (1<<PWM_RESOLUTION), 100},    // pwm3 calibration
    {0, (1<<PWM_RESOLUTION), 100},    // pwm4 calibration
};

preset_t default_preset[] = {
    {1.2f, 0.3f},
    {1.8f, 0.1f},
    {2.5f, 0.1f},
    {3.3f, 1.6f},
    {5.0f, 0.3f},
    {9.6f, 0.05f}
};

// ch1, ch2, ......
static const uint8_t adc_seq[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

static void mapAndSetPwm(float x, float in_max, float in_min, uint8_t ch){
    uint16_t pwm_value = (x - in_min) * (psu.pwm_cal[ch].max - psu.pwm_cal[ch].min) / (in_max - in_min) + psu.pwm_cal[ch].min;
    PWM_Set(ch, pwm_value);
}

extern "C" void psu_adc_cb(uint16_t *data){
    psu.ptr = data;
    SET_AD_FLAG;
}

/**
 * PSU public control functions
 * */
void psu_setOutputVoltage(float val){
    mapAndSetPwm(val, MAX_VOLTAGE, MIN_VOLTAGE, PWM_CH_VOLTAGE);
}

void psu_setOutputCurrent(float val){
    mapAndSetPwm(val, MAX_CURRENT, MIN_CURRENT, PWM_CH_CURRENT);
}

void psu_setInputLoad(float val){
    mapAndSetPwm(val, MAX_LOAD, MIN_LOAD, PWM_CH_LOAD);
}

void psu_setOutputEnable(uint8_t en){    
    if(en){
        SET_OE_FLAG;
        DRAW_Icon(OUTPUT_ICON_POS, (uint8_t *)icon_out, RED);
    }else{
        CLR_OE_FLAG;
        DRAW_FillRect(OUTPUT_ICON_POS, icon_out[0], icon_out[1], BLACK);        
    }
}

float psu_getVoltage(void){
    return *(uint16_t*)psu.ptr * VOLTAGE_PRECISION;
}

float psu_getCurrent(void){
    return *((uint16_t*)psu.ptr + 1) * CURRENT_PRECISION;
}

float psu_getLoadCurrent(void){
    return 0.0f;
}

uint8_t psu_getOutputEnable(void){
    return  GET_OE_FLAG;
}

uint8_t psu_AdcReady(void){
    return GET_AD_FLAG;
}

preset_t *app_getPreset(void){
    return psu.preset;
}

preset_t *app_getPresetList(void){
    return (preset_t*)default_preset;
}

void app_setPreset(preset_t *preset){
    psu.preset = preset;
}
/**
 * Application api
 * */
void app_selectMode(uint8_t mode){
    
    if(mode >= MAX_MODES){
        return;
    }
    psu.cur_mode = mode;    
    (modes[mode])->init();
    // Mode clears screen, so must redraw output icon,
    // only if active
    psu_setOutputEnable(GET_OE_FLAG);
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

    psu.pwm_cal = default_pwm_calibration;
    psu.preset = default_preset;
    
    //DBG_DUMP_LINE((uint8_t*)&psu, sizeof(default_eeprom_data), 0);
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
 * Called every 10ms by Timer4 interrupt, as console
 * may block the main thread, having a secondary loop
 * ensures operation of lcd update and button handling
 * */
void tskPsu(void *ptr){
static TickType_t xLastWakeTime;    
uint8_t count = 0;

    app_selectMode(psu.cur_mode);

    ADCMGR_Start();

    LCD_Bkl(TRUE);

    while(1){
        app_checkButtons();
        //DBG_PIN_HIGH;
        modes[psu.cur_mode]->process();
        //DBG_PIN_LOW;        
        if(GET_AD_FLAG){
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

    stdio_port->init();    

    console.init(stdio_port, CONSOLE_PROMPT);
    console.registerCommandList(commands);

    #ifdef ENABLE_DEBUG
    dbg_init(stdio_port);
    #endif
    // wait for usb to start if connected
    vTaskDelay(pdMS_TO_TICKS(500));

    console.cls();

    while(1){
        console.process();
    }
}

extern "C" void app_setup(void){  
    BOARD_Init();
    TEXT_Init();
    NV_Init();

    app_InitEEPROM();

    PWM_Init();

    ADCMGR_Init();

    ADCMGR_SetSequence((uint8_t*)adc_seq, sizeof(adc_seq), psu_adc_cb);

    psu_setOutputEnable(FALSE);

    psu_setOutputVoltage(psu.preset->v);

    psu_setOutputCurrent(psu.preset->i);

    xTaskCreate( tskCmdLine, "CLI", configMINIMAL_STACK_SIZE * 4, &stdio_ops, PRIORITY_LOW, NULL );
    xTaskCreate( tskPsu, "PSU", configMINIMAL_STACK_SIZE * 4, NULL, PRIORITY_LOW + 1, NULL );
}


