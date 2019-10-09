#ifndef _cmdio_h_
#define _cmdio_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdIo : public ConsoleCommand {
	Console *console;
public:
    CmdIo() : ConsoleCommand("io") {}	
	void init(void *params) { console = static_cast<Console*>(params); }

	void help(void) {
		
	}
	char execute(void *ptr);
};


#ifdef __cplusplus
}
#endif

#endif