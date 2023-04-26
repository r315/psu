#ifndef _cmdset_h_
#define _cmdset_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdSet : public ConsoleCommand {
	Console *console;
public:
    CmdSet() : ConsoleCommand("set") {}	
	void init(void *params) { console = static_cast<Console*>(params); }

	void help(void);

	char execute(int argc, char **argv);
};


#ifdef __cplusplus
}
#endif

#endif