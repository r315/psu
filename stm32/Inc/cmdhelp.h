#ifndef _cmdhelp_h_
#define _cmdhelp_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class ConsoleHelp : public ConsoleCommand {
	Console *console;
public:
    ConsoleHelp() : ConsoleCommand("help") {}	
	void init(void *params) { console = static_cast<Console*>(params); }

	void help(void) {
		console->puts("Available commands:\n\n");
		
		for (uint8_t i = 0; i < console->getCmdListSize(); i++) {			
				console->print("\t%s\n", console->getCmdIndexed(i)->getName());
		}
		console->putc('\n');
	}

	char execute(void *ptr) {
		help();
		return CMD_OK;
	}	
};


#ifdef __cplusplus
}
#endif

#endif