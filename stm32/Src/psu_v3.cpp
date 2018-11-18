#include "stm32f1xx_hal.h"
#include "common.h"
#include "tim.h"

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
 * Called every 10ms
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

    HAL_TIM_Base_Start_IT(&htim4);

    console.addCommand(&vdro);
    console.addCommand(&adc1);

    while(1){
        console.process();

       
    }
}


