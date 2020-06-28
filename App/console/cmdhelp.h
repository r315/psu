#ifndef _cmdhelp_h_
#define _cmdhelp_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdHelp : public ConsoleCommand {
	Console *console;
public:
    CmdHelp() : ConsoleCommand("help") {}	
	void init(void *params) { console = static_cast<Console*>(params); }

	void help(void) {
		console->xputs("Available commands:\n\n");
		
		for (uint8_t i = 0; i < console->getCmdListSize(); i++) {			
				console->print("\t%s\n", console->getCmdIndexed(i)->getName());
		}
		console->xputchar('\n');
	}

	char execute(void *ptr) {
		help();
		return CMD_OK;
	}	
};

class CmdReset : public ConsoleCommand {
public:
    CmdReset() : ConsoleCommand("reset") {}	
	void init(void *params) { }

	void help(void) { }

	char execute(void *ptr) {
		NVIC_SystemReset();
		return CMD_OK;
	}	
};


#ifdef __cplusplus
}
#endif

#endif