#include "cmddfu.h"
#include "board.h"

void CmdDfu::help(void){ 
//console->print("Adc Command\n");
}

char CmdDfu::execute(void *ptr){


    console->print("Entering DFU mode\n");
    LCD_ClrArea(0, 0, LCD_W, LCD_H);
    LCD_Update();

	ENABLE_DFU_MAGIC;
    NVIC_SystemReset();
    
    return CMD_OK; 
}