#ifndef _COMMON_H_
#define _COMMON_H_

#include "system.h"
#include "display.h"

#define FULL_FONT
#define FONT_W 6

#define MENU_START_COL 4
#define MENU_PAGE 7	

#define BIG_DIGIT 1
#define NORMAL_DIGIT 0

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
void printInt(uchar c, uchar p, unsigned int value);
void printDecimal(uchar x, uchar y, uchar bigdigit, unsigned int val, unsigned int magnitude, uchar dot);
uchar printTextAtr(uchar c, uchar p, const char *text, uchar atr);
void printText(uchar c, uchar p, const char *text);
struct MenuItem *selectMenuItem(struct MenuItem *items, uchar nitems);
void clrMenu(void);
void clrCanvas(void);
void drawMenuItem(menuitem *mitem);
void drawFrame(const char *title, uchar col);
void printVoltage(uchar c, uchar p, unsigned int mv);
void printCurrent(uchar c, uchar p, unsigned int ma);
void drawSetIcon(uchar c1, uchar p1);
void clrSetIcon(uchar c1, uchar p1);
#endif
