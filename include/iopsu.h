#ifndef _iopsu_h_
#define _iopsu_h_

#define MINVOUT 22
#define MAXVOUT 255
#define MINIOUT 0
#define MAXIOUT 255

#define V1_CH 3     // AN3
#define V2_CH 8     // AN8
#define I1_CH 2     // AN2
#define FVR_CH 0x1F // AN31
#define VSET_CH 0   // PWM1
#define ISET_CH 1   // PWM2

#define BPORT   PORTC
#define BPORTMask 0x07

#define L_KEY (1<<2)	//RB2
#define R_KEY (1<<0)	//RB0
#define M_KEY (1<<1)	//RB1
#define LOAD_EN RB4
#define TRISB_VALUE 0b11101111
#define TRISC_VALUE 0b10000001

/* Reference definitions*/
#define FVR_4096 3
#define FVR_2048 2
#define FVR_2024 1
#define FVR_OFF 0
#define VCONST 60  // ver teoria
#define ICONST 6

#define PWMFREQ 31300

//#define Tosc SYS_FREQ/4UL


void systemInit(void); /* Handles clock switching/osc initialization */
char scanKeys(void); 
char keydown(char key);
int getVout(void);
int getIout(void);
void setFpwm(unsigned int fpwm);
void setDuty(char ch, int duty);
int ADIN(char ch);
#endif
