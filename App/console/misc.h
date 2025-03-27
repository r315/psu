#ifndef _cmd_pwr_h_
#define _cmd_pwr_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"
#include "board.h"
#include "psu.h"


class CmdPwr : public ConsoleCommand{
    Console *console;
public:
    void init(void *params) { console = static_cast<Console*>(params); }
    CmdPwr () : ConsoleCommand("pwroff") { }
    char execute(int argc, char **argv){
        //app_saveState();
        psu_poweroff();
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
	char execute(int argc, char **argv){
        uint8_t data;

        data = ioexp->read(PSU_I2C_BUS);
        console->printf("IO: b%08b\n", data);
        console->printf("PWR: %s\n", PWR_BTN_READ > PWR_BTN_PRESSED ? "ON" : "OFF");

        return CMD_OK;
    }
};


#ifdef __cplusplus
}
#endif

#endif