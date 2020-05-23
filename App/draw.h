#ifndef _draw_h_
#define _draw_h_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SCRATCH_BUF_SIZE    512

extern uint16_t *scratch;

void DRAW_Tile(uint16_t x, uint16_t y, uint16_t *data, uint16_t w, uint16_t h);
void DRAW_Bitmap(uint16_t x, uint16_t y, uint16_t *data);
void DRAW_Icon2Bitmap(uint16_t *bitmap, uint8_t *icon, uint16_t fcolor, uint16_t bcolor);
void DRAW_Icon(uint16_t x, uint16_t y, uint8_t *ico, uint16_t color);
void DRAW_Icon2(uint16_t x, uint16_t y, uint8_t *ico, uint16_t fcolor, uint16_t bcolor);
void DRAW_FillRect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h, uint16_t color);

#ifdef __cplusplus
}
#endif

#endif