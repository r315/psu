#ifndef _cmd_pwm_h_
#define _cmd_pwm_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"

class CmdPwm : public Command{

public:
    CmdPwm () : Command("pwm") { }
    char execute(void *ptr);
    void help(void);
};


#ifdef __cplusplus
}
#endif

#endif