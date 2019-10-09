
#include "cmdio.h"
#include "board.h"

char CmdIo::execute(void *ptr){
uint8_t data;
    //if(!nextHex((char**)&ptr, &pdata)){
    //    return CMD_BAD_PARAM;
    //}

    data = EXPANDER_Read();

    console->print("IO: b%b\n", data);

    return CMD_OK;
}