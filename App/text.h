#ifndef _text_h_
#define _text_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <font.h>

#define MAX_LINE_CHARS 16
#define DEFAULT_FONT        &defaultFont

void TEXT_Init(void);
void TEXT_Print(uint16_t, uint16_t, const char*);
uint8_t TEXT_Char(uint16_t x, uint16_t y, uint8_t c);
void TEXT_TileChar(uint16_t *tile, uint8_t c);
void TEXT_SetFont(font_t *font);
void TEXT_SetPalette(uint16_t *palette);

extern font_t font_seven_seg;
extern font_t defaultFont;

#ifdef __cplusplus
}
#endif


#endif