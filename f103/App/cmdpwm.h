#ifndef _cmd_pwm_h_
#define _cmd_pwm_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

class CmdPwm : public ConsoleCommand{
    Console *console;
public:
    void init(void *params) { console = static_cast<Console*>(params); }
    CmdPwm () : ConsoleCommand("pwm") { }
    char execute(void *ptr);
    void help(void);
};

/**
 * PWM Output
 * 
 * PINS: 
 * PA6 PWM1
 * PA7 PWM2
 * PB0 PWM3
 * PB1 PWM4
 * 
 * Frequency
 * 35.09kHz (10bit resolution)
 * 
 * */


#ifdef __cplusplus
}
#endif

#endif