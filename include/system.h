#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#ifdef _WIN32
	#include "ioemulator.h"
#else
	#include "iopsu.h"
#endif

typedef unsigned char uchar;

char done(void);
char scanKeys(void);
char keyDown(char key);
char readKeysUpdate(uchar max, uchar min, uchar *var);
char getKey(void);

void delayMs(char ms);


void setDuty(char ch, int duty);
void enableLoad(char state);
#endif
