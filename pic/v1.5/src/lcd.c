/*
 *  KS0713 library
 *  Hugo Reis 2013
 *
 * This software is free you can edit modify or redistribute it, under the 
 * terms of General Public License 
 * 
 * This software is distributed WITHOUT ANY WARRANTY
 */

#include <htc.h>
#include <stdarg.h>
#include "lcd.h"

void DelayMs(unsigned char cnt);

void sout(char bits) 
{
	SSPBUF = bits;
	while(!BF);
}
//-----------------------------------------------
// 
//-----------------------------------------------
void lcdcmd(char c)
{
	LCDRS0
	sout(c);
}
//-----------------------------------------------
// 
//-----------------------------------------------
void lcddata(char d)
{
	LCDRS1
	sout(d);
}
//-----------------------------------------------
//
//-----------------------------------------------
void lcdsetPos(char x, char y)
{
	lcdcmd(0x10 | (x>>4));
	lcdcmd(x&0x0f);
	lcdcmd(0xB0 + y);
}
//-----------------------------------------------
//
//-----------------------------------------------
void lcdScroll(char line)
{
	line &= 0x3F;
	lcdcmd(INTL | line);
}
//-----------------------------------------------
//
//-----------------------------------------------
void lcdInit(void)
{
unsigned w,h;

	LCDRST0	
    lcdcmd(0xFF);
	DelayMs(250);
	LCDRST1

	lcdcmd(0xA0);  // ADC select
	lcdcmd(0xC8);  // SHL Select
	lcdcmd(0xA3);  // select bias	
	
	lcdcmd(0x2C);  // voltage converter on
	DelayMs(2);
	lcdcmd(0x2E);  // voltage regulator on
	DelayMs(2);
	lcdcmd(0x2F);  // voltage follower on
	
	lcdcmd(0x27);  // regulator resistor
	lcdcmd(0x81);  // set reference voltage 
	lcdcmd(0x26);  // reference voltage value (contraste)
	
	lcdcmd(0xAF);  // DiSP ON	

	//lcdcmd(0xA5);  // all on
	//for(h=0;h<8;h++)
	//{
	//	lcdsetPos(0,h);
	//	for(w=0;w<LCD_W;w++)
	//		lcddata(0x0);
	//}
	//
	//lcdcmd(0xA4);  // all off
	lcdsetPos(0,0);
}
