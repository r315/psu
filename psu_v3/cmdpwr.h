#ifndef _cmd_pwr_h_
#define _cmd_pwr_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include <console.h>


class CmdPwr : public ConsoleCommand{
    Console *console;
public:
    void init(void *params) { console = static_cast<Console*>(params); }
    CmdPwr () : ConsoleCommand("pwroff") { }
    char execute(void *ptr){
            SPOWER_OFF;                
    }
    void help(void){}
}; 

#ifdef __cplusplus
}
#endif

#endif