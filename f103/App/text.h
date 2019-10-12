#ifndef _text_h_
#define _text_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <font.h>


#define MAX_LINE_CHARS 16
#define DEFAULT_FONT &font_default //&font_seven_seg


void SEVEN_Double(uint16_t x, uint16_t y, double val);
void SEVEN_Init(void);
void SEVEN_PrintAmps(double amps);
void SEVEN_PrintVolts(double volts);


void TEXT_print(uint16_t, uint16_t, const char*, ...);
void TEXT_Init(void);
void TEXT_setFont(font_t *font);
void TEXT_dro(uint16_t x, uint16_t y, float val, uint8_t precision, int8_t blank_place);
void TEXT_drawGfx(uint16_t x, uint16_t y, uint8_t *data);

extern font_t font_seven_seg;
extern font_t font_default;
#ifdef __cplusplus
}
#endif


#endif