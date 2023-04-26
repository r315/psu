#include "cmddfu.h"
#include "board.h"
#include "draw.h"

#ifdef ENABLE_DFU

void CmdDfu::help(void){ 
//console->print("Adc Command\n");
}

char CmdDfu::execute(int argc, char **argv){


    console->print("Entering DFU mode\n");
    DRAW_FillRect(0, 0, LCD_W, LCD_H, LCD_BLACK);
	ENABLE_DFU_MAGIC;
    NVIC_SystemReset();
    
    return CMD_OK; 
}

#endif