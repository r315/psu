#ifndef _cmdstatus_h_
#define _cmdstatus_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <console.h>

class CmdStatus : public ConsoleCommand {
	Console *console;
public:
	void init(void *params) { console = static_cast<Console*>(params); }
    CmdStatus() : ConsoleCommand("status") {}	

	void help(void);
    
	char execute(void *ptr);
};

#ifdef __cplusplus
}
#endif

#endif