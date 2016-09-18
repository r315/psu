#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#ifdef _WIN32
	#include "ioemulator.h"
#else
	#include "iopsu.h"	
#endif

typedef unsigned char uchar;
typedef unsigned int uint;

typedef struct Mesure{
	uchar adc_ch1;
	uchar adc_ch2;
	uint ch1_voltage;
	uint ch2_voltage;
	uint current;
	uint power;
}mesure;

extern bank1 mesure outvalues;

#define MIN_VOUT_PWM_VAL 22
#define MAX_VOUT_PWM_VAL 255
#define MIN_IOUT_PWM_VAL 0
#define MAX_IOUT_PWM_VAL 255

#define MAX_VOUT 13000
#define MIN_VOUT 1200
#define MAX_IOUT 1530
#define MIN_IOUT 0

#define VCONST 60 
#define ICONST 6

#define VSET_CH 1  // PWM2
#define ISET_CH 0  // PWM1
#define VOUT_CH1 0 // AN
#define VOUT_CH2 4 // AN
#define IOUT_CH  2 // AN

char done(void);
char scanKeys(void);
char keyDown(char key);
char filterKey(void);
char scanKeysAndUpdateValue(uchar max, uchar min, uchar *var);
char updateValueForKey(uchar max, uchar min, uchar *var);

void delayMs(char ms);
void systemInit(void);

void setDuty(char ch, int duty);
void enableLoad(char state);
void finish(void);
int getVout(char ch);
int getIout(void);
void getMesures(mesure *msr);
void disableOutput(void);
#endif
