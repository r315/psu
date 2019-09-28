#ifndef _text_h_
#define _text_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct _font_t{
    uint8_t width;
    uint8_t height;
    const uint8_t *chartable;
    uint8_t bpl;        // Bytes per line
    uint8_t offset;     // offset from ascii table
    uint8_t spacing;
}font_t;

#define MAX_LINE_CHARS 16
#define DEFAULT_FONT &font_default //&font_seven_seg


void SEVEN_Double(uint16_t x, uint16_t y, double val);
void SEVEN_Init(void);
void SEVEN_PrintAmps(double amps);
void SEVEN_PrintVolts(double volts);


void TEXT_print(uint16_t, uint16_t, const char*, ...);
void TEXT_Init(void);
void TEXT_setFont(font_t *font);

extern font_t font_seven_seg;
extern font_t font_default;
#ifdef __cplusplus
}
#endif


#endif