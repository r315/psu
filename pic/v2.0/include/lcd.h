/*
 *  KS0713 library
 *  Hugo Reis 2013
 *
 * This software is free you can edit modify or redistribute it, under the 
 * terms of General Public License 
 * 
 * This software is distributed WITHOUT ANY WARRANTY
 */

#ifndef _KS0713
#define _KS0713
/*******************************************
 * Board - pro59801 
 * Controller - KS0713(S6B1713)
 * LCD - BP12864A
 * Mode - serial
 * 
 * 1 GND
 * 2 VCC
 * 3 NC
 * 4 /RSTB   - RST
 * 5 /CS1    - GND
 * 6 RS
 * 7 RW_WR   - GND
 * 8 E_RD    - GND
 * 9-14 DB0-DB5 
 * 15 DB6 <- SCK
 * 16 DB7 <- SDO
 * 17 led+
 * 18 led-
 *******************************************/

//#define RESET  0xE2
#define DISP   0xAE  // display on/off
#define INTL   0x40  // initial display line
#define RMODE  0x81  // set reference mode
#define PASET  0xB0
#define CASET  0x10  // MSB
#define ADC_SEL  0xA0  //
#define SHL_SEL  0xC0
#define LCD_BIAS 0xA2
#define LCD_PCON 0x28
#define LCD_RES  0x20
#define REV    0xA6
#define EON    0xA4

#define LCD_W 128
#define LCD_H 64
#define LCD_PAGES (LCD_H/8)

#define LCDCLK RC3
#define LCDRS  RC4
#define LCDSDI RC5
#define LCDRST RC6

#define LCDSDI0 LCDSDI = 0;
#define LCDSDI1 LCDSDI = 1;
#define LCDCLK0 LCDCLK = 0;
#define LCDCLK1 LCDCLK = 1;
#define LCDRS0  LCDRS  = 0;
#define LCDRS1  LCDRS  = 1;
#define LCDRST0 LCDRST = 0;
#define LCDRST1 LCDRST = 1;
#define lcdUpdate() 

void lcdInit(void);
void lcdcmd(char c);
void lcdData(char d);
void lcdsetPos(char x, char y);
void lcdScroll(char line);
void solidFill(char d);

#endif
