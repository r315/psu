#ifndef _iopsu_h_
#define _iopsu_h_

#include <htc.h>

#define BPORT   PORTB
#define BPORTMask 0x07
#define L_KEY (1<<2)	//RB2
#define R_KEY (1<<0)	//RB0
#define M_KEY (1<<1)	//RB1
#define K_HOLD (1<<7)   //Key Hold Flag

#define LOAD_EN RB4
#define TRISA_VALUE 0b11111111
#define TRISB_VALUE 0b11101111
#define TRISC_VALUE 0b10000001

#define Tosc PIC_CLK/4UL
#define CCP_PWM 0x0C
#define MIN_PR2 0x40
#define PWMFREQ 31300
#define PIC_CLK 8000000

//#define Tosc SYS_FREQ/4UL
void setFpwm(unsigned int fpwm);
void setDuty(char ch, int duty);
#endif
