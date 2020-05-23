#include "cmddfu.h"
#include "board.h"
#include "draw.h"

void CmdDfu::help(void){ 
//console->print("Adc Command\n");
}

char CmdDfu::execute(void *ptr){


    console->print("Entering DFU mode\n");
    DRAW_FillRect(0, 0, LCD_W, LCD_H, BLACK);
	ENABLE_DFU_MAGIC;
    NVIC_SystemReset();
    
    return CMD_OK; 
}