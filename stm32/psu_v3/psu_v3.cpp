#include "common.h"
#include "tim.h"

/**
 * HW modules configuration
 * TIM4   program loop tick
 * TIM2   ADC Trigger
 * DMA1   ADC data transfer
 * ADC1-2 Dual configuration for simultaneous convertion
 * TIM3   PWM signals
 * */

Console console;

class VoltageDro : public Command{
    double v = 0;
public:
    char execute(void *ptr){
        SEVEN_Double(1,1,v);
        v += 0.5;
        return CMD_OK;
    }
    void help(void){}

    VoltageDro(void) : Command("vdro") {}
};

enum Mode{
    PSU,
    LOAD
};

enum State{
    RUN
};

class Instrument{
    uint32_t ticks;
public:
    double vout;
    Mode mode;
    State state;

    uint32_t isTimed(void){
        if(ticks == 0)
            return YES;

        return !(--ticks);
    } 

};

static Instrument psu;
volatile int done = 0;

void UpdateResult(uint16_t *adcres){
    SEVEN_Double(1,1, (3.3 * adcres[2]) / 4096);
    SEVEN_Double(65,1, (3.3 * adcres[3]) / 4096);
    LCD_Update();
    done += 1;
}



void handleButtons(void){
    BUTTON_Read();
    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        switch(BUTTON_GetValue()){
            case BUTTON_LEFT:
                psu.vout += 0.01;
                break;

            case BUTTON_RIGHT: 
                psu.vout -= 0.01;
                break;
        }
        SEVEN_Double(1,1,psu.vout);
        LCD_Update();
    }
}

/**
 * Called every 10ms by Timer4 interrupt
 * */
extern "C" void psu_v3_loop(void){
    
    handleButtons();
     
    /*    switch(psu.mode){
            case PSU: break;
            case LOAD: break;
        }
    LCD_Update();*/

    HAL_GPIO_TogglePin(GPIOA, DBG_Pin);
}

extern "C" void psu_v3(void){
VoltageDro vdro;
CmdAdc adc1;
uint16_t pwm_start_values [] = { 0x80, 0x180, 0x280, 0x380};

    HAL_TIM_Base_Start_IT(&htim4); // start loop

    ADC_Init(100);
    ADC_SetCallBack(UpdateResult);

    PWM_Init(pwm_start_values);

    console.addCommand(&vdro);
    console.addCommand(&adc1);

    while(1){
        console.process();       
    }
}


