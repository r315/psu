#include "psu.h"

char CmdOut::execute(void *ptr) {
    uint32_t pdata;
    if(!nextHex((char**)&ptr, &pdata)){
        console->print("usage: out <state>\n\tstate: 0 - off, 1 - on\n");
        return CMD_BAD_PARAM;
    }

    if(pdata == 1){
        setOutputEnable(1);
    }else{
        setOutputEnable(0);
    }
    
    return CMD_OK;
}	