
#include "psu.h"


/**
 * HW modules configuration
 * TIM4     HAL Timer
 * TIM2     ADC Trigger
 * DMA1-CH1 ADC data transfer
 * DMA1-CH4 I2C data transfer
 * ADC1-2   Dual configuration for simultaneous convertion
 * TIM3     PWM signals
 * */

static State psu_state;

static ModePsu cpsu;
static ModeLoad cload;
static Mode *modes[] = {
    &cpsu,
    &cload      
};

};

static void mapAndSetPwm(float x, float in_max, float in_min, uint8_t ch){
uint16_t pwm_value = (x - in_min) * (psu_state.cal_data[ch].max - psu_state.cal_data[ch].min) / (in_max - in_min) + psu_state.cal_data[ch].min;
    PWM_Set(PWM_CH_VOLTAGE, pwm_value);
}

void setOutputVoltage(float val, float max, float min){
    mapAndSetPwm(val, max, min, PWM_CH_VOLTAGE);
}

void setOutputCurrent(float val, float max, float min){
    mapAndSetPwm(val, max, min, PWM_CH_CURRENT);
}

void setOutputEnable(uint8_t en){
    psu_state.output_en = en;
    if(psu_state.output_en){
        TEXT_drawGfx(OUTPUT_ICON_POS, (uint8_t*)&icon_out[0]);
    }else{
        LCD_Fill(OUTPUT_ICON_POS, icon_out[0], icon_out[1], BLACK);
    }
}

void selectMode(uint8_t mode){
    
    if(mode >= MAX_MODES){
        return;
    }

    psu_state.mode_select = mode;    
    (modes[mode])->redraw();
    // Mode clears screen, so must redraw output icon,
    // only if active
    if(psu_state.output_en)
        setOutputEnable(psu_state.output_en);
}

void cycleMode(void){
uint8_t mode = psu_state.mode_select + 1;
    
    if(mode == MAX_MODES ){
        mode = 0;
    }
    
    selectMode(mode);
}

void toggleOutput(void){
    setOutputEnable(!psu_state.output_en);
}

void checkButtons(){
    BUTTON_Read();
    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        switch(BUTTON_VALUE){
            case BUTTON_MODE: cycleMode(); break;
            case BUTTON_OUT: toggleOutput(); break;
            case BUTTON_SET: modes[psu_state.mode_select]->modeSet(); break;
        }
    }
}

/**
 * ADC End of convertion callback
 * */
void adcEocCb(uint16_t *res){
    psu_state.adcvalues = *((uint64_t*)res);
}

/**
 * Called every 10ms by Timer4 interrupt, as console
 * may block the main thread, having a secondary loop
 * ensures operation of lcd update and button handling
 * */
void tskPsu(void *ptr){
static TickType_t xLastWakeTime;
    TEXT_Init();

    selectMode(0);
    ADC_SetCallBack(adcEocCb);   

    while(1){
        checkButtons();
        DBG_LED_ON;
        modes[psu_state.mode_select]->process(&psu_state);
        DBG_LED_OFF;
        LCD_Update();
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(UPDATE_INTERVAL));
    }
}

void tskConsole(void *ptr){
Console console;
CmdHelp help;
CmdAdc adc1;
CmdPwm pwm;
CmdDfu dfu;
CmdPwr pwr;
CmdOut out;
CmdIo io;
    
    vcom.init();    
    console.init(&vcom, CONSOLE_PROMPT);
    console.addCommand(&help);
    console.addCommand(&adc1);
    console.addCommand(&pwm);
    console.addCommand(&dfu);
    console.addCommand(&pwr); 
    console.addCommand(&out); 
    console.addCommand(&io); 

    while(1){
        console.process();
    }
}

extern "C" void psu(void){  

    PWM_Init((uint16_t*)pwm_start_values);
    ADC_Init(ADC_INTERVAL);   

    xTaskCreate( tskConsole, "CLI", configMINIMAL_STACK_SIZE * 4, NULL, 3, NULL );
    xTaskCreate( tskPsu, "PSU", configMINIMAL_STACK_SIZE * 4, NULL, 3, NULL );
}


