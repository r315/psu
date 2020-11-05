#ifndef _cmd_pwr_h_
#define _cmd_pwr_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <console.h>
#include "board.h"
#include "psu.h"


class CmdPwr : public ConsoleCommand{
    Console *console;
public:
    void init(void *params) { console = static_cast<Console*>(params); }
    CmdPwr () : ConsoleCommand("pwroff") { }
    char execute(void *ptr){
        app_poweroff();
        return CMD_OK;
    }
    void help(void){}
}; 


class CmdIo : public ConsoleCommand {
	Console *console;
public:
    CmdIo() : ConsoleCommand("keys") {}	
	void init(void *params) { console = static_cast<Console*>(params); }

	void help(void) {
		
	}
	char execute(void *ptr){
        uint8_t data;

        data = EXPANDER_Read();

        console->print("IO: b%08b\n", data);
        console->print("PWR: %s\n", GET_PWR_BTN ? "ON" : "OFF");

        return CMD_OK;
    }
};


#ifdef __cplusplus
}
#endif

#endif