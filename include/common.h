#ifndef _COMMON_H_
#define _COMMON_H_

#define FULLFONT 
#include "font.h"
#include "system.h"

#define MENU_START 4

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

void printChar(char c, char atr);
void printString(string *str);
void printInt(uchar c, uchar p, unsigned int value);
void printBigDecimal(char x, char y, unsigned int val, unsigned int div, char dot);
void printText(char c, char p, const char *text);
struct MenuItem *selectMenuItem(struct MenuItem *items, char nitems, uchar spacing);
void clrMenu(void);
void drawMenuItem(menuitem *mitem);
void drawFrame(string *title);
#endif
