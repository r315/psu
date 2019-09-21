#include "cmddfu.h"
#include "board.h"

extern uint32_t _estack;

void CmdDfu::help(void){ 
//console->print("Adc Command\n");
}

char CmdDfu::execute(void *ptr){
uint64_t *dfu_magic = (uint64_t*)&_estack;

    console->print("Entering DFU mode\n");
    LCD_ClrArea(0, 0, LCD_W, LCD_H);
    LCD_Update();

	*(dfu_magic - 1) = 0xDEADBEEFCC00FFEEULL;
    NVIC_SystemReset();
    
    return CMD_OK; 
}