#include "stm32f1xx_hal.h"
#include "console.h"
#include "command.h"

Console console;

extern "C" void psu_v3(void){
     //(dynamic_cast<Console*>(_this))->print("Hello\n");   
	 //(dynamic_cast<Console*>(_this))->vcom.putString("Hello\n", 6);

    console.init();

    while(1){
        console.vcom.putString("\rHello\n",7);
        HAL_Delay(500);
    }
}


