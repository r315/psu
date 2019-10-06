#ifndef _ssd1306_h_
#define _ssd1306_h_

#include <stdint.h>

#define SSD1306_I2C_ADDRESS   			0x3C // 8-bit address

#define SSD1306_LCDWIDTH                128
#define SSD1306_LCDHEIGHT               32

#define SSD1306_NOP			 			0xE3

#define SSD1306_SETCONTRAST 			0x81
#define SSD1306_DISPLAYALLON_RESUME		0xA4
#define SSD1306_DISPLAYALLON 			0xA5
#define SSD1306_NORMALDISPLAY 			0xA6
#define SSD1306_INVERTDISPLAY 			0xA7
#define SSD1306_DISPLAYOFF 				0xAE
#define SSD1306_DISPLAYON 				0xAF

#define SSD1306_SETDISPLAYOFFSET 		0xD3
#define SSD1306_SETCOMPINS 				0xDA

#define SSD1306_SETVCOMDETECT 			0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 		0xD5
#define SSD1306_SETPRECHARGE 			0xD9

#define SSD1306_SETMULTIPLEX 			0xA8

#define SSD1306_SETLOWCOLUMN 			0x00
#define SSD1306_SETHIGHCOLUMN 			0x10

#define SSD1306_SETSTARTLINE 			0x40

#define SSD1306_MEMORYMODE 				0x20
#define SSD1306_COLUMNADDR 				0x21
#define SSD1306_PAGEADDR   				0x22

#define SSD1306_COMSCANINC 				0xC0
#define SSD1306_COMSCANDEC 				0xC8

#define SSD1306_SEGREMAP 				0xA0

#define SSD1306_CHARGEPUMP 				0x8D

#define SSD1306_EXTERNALVCC 			0x1
#define SSD1306_SWITCHCAPVCC 			0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 		0x2F
#define SSD1306_DEACTIVATE_SCROLL 		0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 	0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

enum{
    BLACK = 0,
    WHITE,
    INVERSE
};

void LCD_ScrollRight(uint8_t start, uint8_t stop);
void LCD_StopScroll(void);

uint16_t LCD_GetWidth(void);
uint16_t LCD_GetHeight(void);

void LCD_Update(void);
void LCD_Fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void LCD_SetFrame(uint16_t c, uint16_t p, uint8_t *data);

void LCD_Init(void);
void LCD_Pixel(uint16_t x, uint16_t y, uint16_t color);

uint8_t *LCD_GetPixels(void);

#endif
