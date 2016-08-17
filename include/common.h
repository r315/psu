#ifndef _COMMON_H_
#define _COMMON_H_

#include "system.h"
#include "display.h"

#define FULL_FONT
#define FONT_W 6

#define MENU_START 4

#define BIG_DIGIT 1
#define NORMAL_DIGIT 0

typedef struct String{
	const char *chars;
	char size;
	char atribute;
	char col;
	char page;
}string;

typedef struct MenuItem{
	const char *name;
	void *data;
	void(*run)(void);
}menuitem;

typedef struct Pwm{
	uchar duty;
	uchar channel;
	uchar minduty;
	uchar maxduty;
}pwm;

void printChar(uchar c, uchar atr);
void printString(string *str);
void printInt(uchar c, uchar p, unsigned int value);
void printDecimal(uchar x, uchar y, uchar bigdigit, unsigned int val, unsigned int magnitude, uchar dot);
void printText(uchar c, uchar p, const char *text);
struct MenuItem *selectMenuItem(struct MenuItem *items, uchar nitems, uchar spacing);
void clrMenu(void);
void clrCanvas(void);
void drawMenuItem(menuitem *mitem);
void drawFrame(string *title);
void printVoltage(uchar c, uchar p, unsigned int mv);
void printCurrent(uchar c, uchar p, unsigned int ma);
void drawSetIcon(uchar c, uchar p);

#endif
