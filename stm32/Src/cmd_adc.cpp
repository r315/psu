

#include "cmd_adc.h"

void CmdAdc::help(void){ 

}

char CmdAdc::execute(void *ptr){ 
    console.print("Adc Command\n");
    return CMD_OK; 
}