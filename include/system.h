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

bank1 static mesure outvalues; //TODO: min ensure current when change modes(PSU/load)

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

char done(void);
char scanKeys(void);
char keyDown(char key);
char getKey(void);
char readKeysAndUpdateValue(uchar max, uchar min, uchar *var);
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
