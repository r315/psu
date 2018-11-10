#ifndef _system_h_
#define _system_h_
/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

/* Microcontroller MIPs (FCY) */
#define SYS_FREQ        8000000L //500000L
#define FCY             SYS_FREQ/4

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

/* Pin definitions */

#define LCDSDI RC7
#define LCDRS  RB5
#define LCDCLK RB6
#define LCDRST RB4

#define LOAD_EN RB7

#define V1_CH 3     // AN3
#define V2_CH 8     // AN8
#define I1_CH 2     // AN2
#define FVR_CH 0x1F // AN31
#define VSET_CH 0   // PWM1
#define ISET_CH 1   // PWM2

#define BPORT   PORTC
#define BPORTMask 0x0F
#define L_KEY (1<<2)
#define R_KEY (1<<0)
#define M_KEY (1<<1)

/* Reference definitions*/
#define FVR_4096 3
#define FVR_2048 2
#define FVR_2024 1
#define FVR_OFF 0
#define VCONST 60  // ver teoria
#define ICONST 6

#define PWMFREQ 31300
#define Tosc SYS_FREQ/4UL


void systemInit(void); /* Handles clock switching/osc initialization */
char scanKeys(void); 
char keydown(char key);
int getVout(void);
int getIout(void);
void setFpwm(unsigned int fpwm);
void setDuty(char ch, int duty);
int ADIN(char ch);
#endif
