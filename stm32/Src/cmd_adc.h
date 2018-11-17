#ifndef _cmd_adc_h_
#define _cmd_adc_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"     // all drivers headers should be placed here 

class CmdAdc : public Command{

public:
    CmdAdc () : Command("adc") { }
    char execute(void *ptr);
    void help(void);
};


#ifdef __cplusplus
}
#endif

#endif