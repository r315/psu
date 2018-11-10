#include "stm32f1xx_hal.h"
#include "console.h"
#include "command.h"
#include "seven_seg.h"


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
Console console;
VoltageDro vdro;

    console.addCommand(&vdro);

    while(1){
	    //(dynamic_cast<Console*>(&console))->process();
        console.process();
        HAL_Delay(20);
        LCD_Update();
    }
}


