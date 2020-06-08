
#include "psu.h"
#include "draw.h"


static State psu_state;
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
static CmdOut out;
static CmdIo io;
static CmdMode mode;
#ifdef ENABLE_DFU
static CmdDfu dfu;
#endif

static ConsoleCommand *commands[] = {
    &help,
    &adc1,
    &pwm,
    &pwr,
    &out,
    &io,
    &mode,
#ifdef ENABLE_DFU
    &dfu,
#endif
    NULL
};

// min, max, start
static calibration_t default_cal_data[PWM_NUM_CH] = { 
    {0, (1<<PWM_RESOLUTION), 100 },
    {0, (1<<PWM_RESOLUTION), 100 },
    {0, (1<<PWM_RESOLUTION), 100 },    
    {0, (1<<PWM_RESOLUTION), 100 },
};

static void mapAndSetPwm(float x, float in_max, float in_min, uint8_t ch){
    uint16_t pwm_value = (x - in_min) * (psu_state.cal_data[ch].max - psu_state.cal_data[ch].min) / (in_max - in_min) + psu_state.cal_data[ch].min;
    PWM_Set(PWM_CH_VOLTAGE, pwm_value);
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
    psu_state.output_en = en;
    
    if(psu_state.output_en){
        DRAW_Icon(OUTPUT_ICON_POS, (uint8_t *)icon_out, RED);
    }else{
        DRAW_FillRect(OUTPUT_ICON_POS, icon_out[0], icon_out[1], BLACK);        
    }
}

/**
 * Application api
 * */
void app_selectMode(uint8_t mode){
    
    if(mode >= MAX_MODES){
        return;
    }

    psu_state.mode = mode;    
    (modes[mode])->init();
    // Mode clears screen, so must redraw output icon,
    // only if active
    if(psu_state.output_en)
        psu_setOutputEnable(psu_state.output_en);
}

void app_cycleMode(void){
uint8_t mode = psu_state.mode + 1;
    
    if(mode == MAX_MODES ){
        mode = 0;
    }
    
    app_selectMode(mode);
}

void app_checkButtons(){
    BUTTON_Read();
    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        switch(BUTTON_VALUE){
            case BUTTON_MODE: app_cycleMode(); break;
            case BUTTON_OUT: psu_setOutputEnable(!psu_state.output_en); break;
            case BUTTON_SET: modes[psu_state.mode]->modeSet(); break;
        }
    }
}

#ifndef USE_ADCMUX
/**
 * ADC End of convertion callback
 * */
static void app_adcEocCb(uint16_t *res){
uint32_t *src = (uint32_t*)res;
    for (uint32_t i = 0; i < ADC_SEQ_LEN; i++, src++)
    {
        psu_state.adcvalues[i] = *src;
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

    app_selectMode(psu_state.mode);
#ifndef USE_ADCMUX
    ADC_SetCallBack(app_adcEocCb);
#endif

    while(1){
        app_checkButtons();
        //DBG_PIN_HIGH;
        modes[psu_state.mode]->process(&psu_state);
        //DBG_PIN_LOW;        
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(UPDATE_INTERVAL));
    }
}

void tskCmdLine(void *ptr){
    
    vcom.init();    
    console.init(&vcom, CONSOLE_PROMPT);
    console.registerCommandList(commands);

    while(1){
        console.process();
    }
}

extern "C" void app_setup(void){  
uint16_t pwm_start_values[PWM_NUM_CH];

    BOARD_Init();
    TEXT_Init();

    memcpy(psu_state.cal_data, default_cal_data, sizeof(calibration_t) * PWM_NUM_CH);    

    for(int i = 0; i < PWM_NUM_CH ; i++)
    {
        pwm_start_values[i] = psu_state.cal_data[i].start;
    }

    cpsu.initPreSetValues(MIN_VOLTAGE, MIN_CURRENT);

    PWM_Init(pwm_start_values);
    ADC_Init(ADC_INTERVAL);

    psu_state.mode = 0;

    xTaskCreate( tskCmdLine, "CLI", configMINIMAL_STACK_SIZE * 4, NULL, 3, NULL );
    xTaskCreate( tskPsu, "PSU", configMINIMAL_STACK_SIZE * 4, NULL, 3, NULL );
}


