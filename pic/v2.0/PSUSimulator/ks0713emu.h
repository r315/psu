#ifndef _KS0713EMU_H_
#define _KS0713EMU_H_

#include <sdl/sdl.h>
#define LCD_W 128
#define LCD_H 64
#define LCD_PAGES (LCD_H/8)



extern SDL_Surface* screen;

void lcdInit(void);
void lcdsetPos(unsigned char x, unsigned char y);
void lcdData(unsigned char data);
void solidFill(char d);
void lcdUpdate(void);
#endif
