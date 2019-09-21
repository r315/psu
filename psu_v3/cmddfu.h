#ifndef _cmd_dfu_h_
#define _cmd_dfu_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <console.h>

class CmdDfu : public ConsoleCommand{
    Console *console;
public:
    void init(void *params) { console = static_cast<Console*>(params); }
    CmdDfu () : ConsoleCommand("dfu") { }
    char execute(void *ptr);
    void help(void);
};


#ifdef __cplusplus
}
#endif

#endif