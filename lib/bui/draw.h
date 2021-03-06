#ifndef _draw_h_
#define _draw_h_

#include <stdint.h>
#include "font.h"

#ifdef __cplusplus
extern "C" {
#endif


void DRAW_Bitmap(uint16_t x, uint16_t y, uint16_t *data);
void DRAW_Icon2Bitmap(uint16_t *bitmap, const uint8_t *icon, const uint16_t *pal);
void DRAW_Icon(uint16_t x, uint16_t y, const uint8_t *ico, const uint16_t *pal);
void DRAW_FillRect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h, uint16_t color);
void DRAW_Rect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h, uint16_t color);

void DRAW_VLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
void DRAW_HLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
void DRAW_Pixel(uint16_t x, uint16_t y, uint16_t color);

void DRAW_WaitOpEnd(void);

uint16_t DRAW_Char(uint16_t x, uint16_t y, uint8_t c, font_t *fnt, const uint16_t *pal);
uint16_t DRAW_Text(uint16_t x, uint16_t y, const char* str, font_t *fnt, const uint16_t *pal);

#ifdef __cplusplus
}
#endif

#endif