#include "cmdadc.h"
#include "board.h"

void CmdAdc::help(void){ 
console->print("Usage: adc\n");
console->print("Command not implemented\n");
}

char CmdAdc::execute(void *ptr){ 
/* uint16_t *buf;
    do{
        console->print("\n");
        buf = ADC_LastConvertion();
        for(uint8_t i = 0; i < 4; i++){
            console->print("ADC[%d] = %u \n", i, buf[i]);
        }      
        DelayMs(400);
    }while(!console->kbhit());
 */   
    return CMD_OK; 
}