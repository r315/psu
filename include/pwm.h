#ifndef _pwm_h_
#define _pwm_h_

#define Tosc PIC_CLK/4UL
#define CCP_PWM 0x0C
#define MIN_PR2 0x40

void setFpwm(unsigned int fpwm);
void setDuty(char ch, int duty);

#endif
