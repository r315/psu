
#include "psu.h"


/**
 * HW modules configuration
 * TIM4   program loop tick
 * TIM2   ADC Trigger
 * DMA1   ADC data transfer
 * ADC1-2 Dual configuration for simultaneous convertion
 * TIM3   PWM signals
 * */

extern StdOut vcom;

class VoltageDro : public ConsoleCommand{
    double v = 0;
public:
    char execute(void *ptr){
        //SEVEN_Double(1,1,v);
        v += 0.5;
        return CMD_OK;
    }
    void help(void){}

    VoltageDro(void) : ConsoleCommand("vdro") {}
};

void UpdateResult(uint16_t *adcres){
static double c = 99.1;
    //SEVEN_Double(1,1, (3.3 * adcres[2]) / 4096);
    //SEVEN_Double(65,1, (3.3 * adcres[3]) / 4096);
    TEXT_dro(VOLTAGE_DRO_POS, c,1);
    c += 0.1;
    if(c == 100.0) c = 0;
    LCD_Update();
}

void handleButtons(void){
    BUTTON_Read();
    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        switch(BUTTON_GetValue()){
            case BUTTON_LEFT:
                
                break;

            case BUTTON_RIGHT: 
                
                break;
        }
        //SEVEN_Double(1,1,sup.vout);
        LCD_Update();
    }
}


extern const uint8_t icon_psu[];
extern const uint8_t icon_load[];
extern const uint8_t icon_out[];
void redrawDisplay(){

    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
    //TEXT_setFont(&defaultBoldFont);
    //TEXT_setFont(&lcdFont);
    TEXT_setFont(&pixelDustFont);
    //TEXT_setFont(&defaultFont);
    TEXT_print(0,0, "88.8W");
    TEXT_setFont(&font_seven_seg);
    TEXT_dro(VOLTAGE_DRO_POS, 88.8, 1);
    TEXT_dro(CURRENT_DRO_POS, 8.88,2);

    TEXT_drawGfx(70,0, (uint8_t*)&icon_psu[0]);
    TEXT_drawGfx(90,0, (uint8_t*)&icon_load[0]);
    TEXT_drawGfx(110,0, (uint8_t*)&icon_out[0]);
    LCD_Update();

}

/**
 * Called every 10ms by Timer4 interrupt, as console
 * may block the main thread, having a secondary loop
 * ensures operation of lcd update and button handling
 * */
extern "C" void psu_v3_loop(void){
    
    handleButtons();
     
    /*    switch(sup.mode){
            case PSU: break;
            case LOAD: break;
        }
    LCD_Update();*/

    HAL_GPIO_TogglePin(GPIOA, DBG_Pin);
}

void tskConsole(void *ptr){
VoltageDro vdro;
Console console;
ConsoleHelp help;
CmdAdc adc1;
CmdPwm pwm;
CmdDfu dfu;
CmdPwr pwr;
    
    vcom.init();    
    console.init(&vcom, "PSU >");
    console.addCommand(&help);

    console.addCommand(&vdro);
    console.addCommand(&adc1);
    console.addCommand(&pwm);
    console.addCommand(&dfu);
    console.addCommand(&pwr); 

    while(1){
        console.process();
    }
}

extern "C" void psu(void){


    TEXT_Init();
    redrawDisplay();

    //PWM_Init((uint16_t*)pwm_start_values);

    //ADC_Init(ADC_INTERVAL);
    //ADC_SetCallBack(UpdateResult);

    //setInterval(psu_v3_loop,APP_INTERVAL);

    //xTaskCreate( tskConsole, "CLI", configMINIMAL_STACK_SIZE, NULL, 3, NULL );

    //vTaskStartScheduler();

    //tskConsole(NULL);
    
    while(1){
        //vTaskDelay(100);
    } 
}


