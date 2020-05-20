#ifndef _cmdout_h_
#define _cmdout_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <console.h>

class CmdOut : public ConsoleCommand {
	Console *console;
public:
    CmdOut() : ConsoleCommand("out") {}	
	void init(void *params) { console = static_cast<Console*>(params); }

	void help(void) { }
	char execute(void *ptr);
};

class CmdMode : public ConsoleCommand {
	Console *console;
public:
    CmdMode() : ConsoleCommand("mode") {}	
	void init(void *params) { console = static_cast<Console*>(params); }

	void help(void) { }
	char execute(void *ptr);
};


#ifdef __cplusplus
}
#endif

#endif