#ifndef _COMMON_H_
#define _COMMON_H_

#include "display.h"
#include "font.h"
#include "system.h"

#define MENU_START 4

typedef struct String{
	char *chars;
	char size;
	char atribute;
	char col;
	char page;
}string;


typedef struct MenuItem{
	char *name;
	uchar var;
	void (*run)(void);
}menuitem;

void printChar(char c, char atr);
void printString(string *str);
void drawFrame(const string *title);
void printBigDecimal(char x, char y, unsigned int val, unsigned int div, char dot);
void printText(char c, char p, const char *text);
struct MenuItem *selectMenuItem(struct MenuItem *items, char nitems, uchar spacing);
#endif
