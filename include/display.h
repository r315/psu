#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#ifdef _WIN32
	#include "ks0713emu.h"	
#else
	#include "lcd.h"	//change name
#endif

#define NORMAL 0
#define INVERTED 255
#define UNDERLINE 128

#endif
