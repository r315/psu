#ifndef _CMDSTATUS_H_
#define _CMFSTATUS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdStatus : public ConsoleCommand {
	Console *console;
public:
    CmdStatus() : ConsoleCommand("status") {}	
	void init(void *params) { console = static_cast<Console*>(params); }

	void help(void){
        console->print("System status\n");
    }

	char execute(void *ptr){
        console->print("Output voltage:\n");
        console->print("Output current:\n");
        console->print("Output enable:\n");
        console->print("Shunt resistor:\n");
        console->print("ADC resolution:\n");

        return CMD_OK;
    }
};


#ifdef __cplusplus
}
#endif

#endif