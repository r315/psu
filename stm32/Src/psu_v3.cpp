#include "stm32f1xx_hal.h"
#include "console.h"
#include "common.h"
#include "seven_seg.h"

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


extern "C" void psu_v3(void){
VoltageDro vdro;
CmdAdc adc1;

    console.addCommand(&vdro);
    console.addCommand(&adc1);

    while(1){
	    
        console.process();
        //HAL_Delay(20);
        LCD_Update();
    }
}


