
#include "psu.h"
#include "draw.h"

static psu_t psu;
static ScreenPsu cpsu;
static ScreenLoad cload;
static ScreenCharger ccharger;
static ScreenPreset preset;

static Screen *modes[] = {
    &cpsu,
    &cload,
    &ccharger,
    &preset
};

static Console console;
static CmdHelp help;
static CmdAdc adc1;
static CmdPwm pwm;
static CmdPwr pwr;
static CmdIo io;
static CmdSet set;
#ifdef ENABLE_DFU
static CmdDfu dfu;
#endif

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
    NULL
};

// min, max, start
static pwmcal_t default_cal_data[PWM_NUM_CH] = { 
    {0, (1<<PWM_RESOLUTION), 100 },
    {0, (1<<PWM_RESOLUTION), 100 },
    {0, (1<<PWM_RESOLUTION), 100 },    
    {0, (1<<PWM_RESOLUTION), 100 },
};

// ch1, ch2, ......
static const uint8_t adc_seq[] = {0 , 1};

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
void psu_setOutputVoltage(float val, float max, float min){
    mapAndSetPwm(val, max, min, PWM_CH_VOLTAGE);
}

void psu_setOutputCurrent(float val, float max, float min){
    mapAndSetPwm(val, max, min, PWM_CH_CURRENT);
}

void psu_setInputLoad(float val, float max, float min){
    mapAndSetPwm(val, max, min, PWM_CH_LOAD);
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
    return *(uint16_t*)psu.ptr + 1 * CURRENT_PRECISION;
}

float psu_getLoadCurrent(void){
    return 0.0f;
}

uint8_t psu_getOutputEnable(void){
    return  GET_OE_FLAG;
}
/**
 * Application api
 * */
void app_selectMode(uint8_t mode){
    
    if(mode >= MAX_MODES){
        return;
    }

    psu.mode = mode;    
    (modes[mode])->init();
    // Mode clears screen, so must redraw output icon,
    // only if active
    psu_setOutputEnable(GET_OE_FLAG);
}

void app_cycleMode(void){
uint8_t mode = psu.mode + 1;
    
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
            case BUTTON_SET: modes[psu.mode]->modeSet(); break;
        }
    }
}

/**
 * @brief
 * 
 * */
void app_InitEEPROM(uint8_t *dst){
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

#ifndef USE_ADCMUX
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

    app_selectMode(psu.mode);

    ADCMUX_Start();

    while(1){
        app_checkButtons();
        //DBG_PIN_HIGH;
        modes[psu.mode]->process(&psu);
        //DBG_PIN_LOW;        
        if(GET_AD_FLAG){
            CLR_AD_FLAG;
            ADCMUX_Start();
        }
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(UPDATE_INTERVAL));
    }
}

void tskCmdLine(void *ptr){
    
    vcom.init();    
    console.init(&vcom, CONSOLE_PROMPT);
    console.registerCommandList(commands);
    
    #ifdef ENABLE_DEBUG
    dbg_init(&vcom);
    #endif

    while(1){
        console.process();
    }
}

extern "C" void app_setup(void){  
uint16_t pwm_start_values[PWM_NUM_CH];

    BOARD_Init();
    TEXT_Init();
    NV_Init();

    app_InitEEPROM(psu.eeprom);

    memcpy(psu.pwm_cal, default_cal_data, sizeof(pwmcal_t) * PWM_NUM_CH);

    cpsu.initPreSetValues(MIN_VOLTAGE, MIN_CURRENT);

    PWM_Init(pwm_start_values);
    ADCMUX_Init();

    ADCMUX_SetSequence((uint8_t*)adc_seq, sizeof(adc_seq), psu_adc_cb);

    psu.mode = 0;

    xTaskCreate( tskCmdLine, "CLI", configMINIMAL_STACK_SIZE * 4, NULL, 3, NULL );
    xTaskCreate( tskPsu, "PSU", configMINIMAL_STACK_SIZE * 4, NULL, 3, NULL );
}


