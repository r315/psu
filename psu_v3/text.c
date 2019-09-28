
#include "text.h"
#include "board.h" 
#include <stdio.h>
#include <stdarg.h>

#define SEVEN_DIG_W     17
#define SEVEN_DIG_H     30

#define SEG_W   9
#define SEG_H   4

#define SEG_A  cx + 4,  cy + 0, segments
#define SEG_B  cx + 13, cy + 4, segments + (SEG_W * SEG_H) + 2
#define SEG_C  cx + 13, cy + 17, segments + (SEG_W * SEG_H) + 2
#define SEG_D  cx + 4,  cy + 26, segments
#define SEG_E  cx + 0,  cy + 17, segments + (SEG_W * SEG_H) + 2
#define SEG_F  cx + 0,  cy + 4, segments + (SEG_W * SEG_H) + 2
#define SEG_G  cx + 4,  cy + 13, segments
#define SEG_DP cx + 17, cy + 27, segments + (2 * ((SEG_W * SEG_H) + 2))

#define UNIT_VOLT     56,0, (uint8_t*)units
#define UNIT_AMP      123,0, (uint8_t*)(units + 6)

static font_t *font;

void TEXT_setFont(font_t *fnt){
    font = fnt;
}

/**
 * draws a character using a font table.
 * the font table must be 1bpp
 */
static uint8_t drawChar(uint16_t x, uint16_t y, uint8_t c){
    const uint8_t *p;
    
    c -= font->offset;
    p = font->data + (c * font->h * font->bpl);

    if(p > font->data + font->data_len)
        return;

    for(uint16_t h = y; h < y +font->h; h++, p++){
        uint8_t line = *p;
        for(uint16_t w = x, bc = 0; w < x + font->w; w++, bc++){
            if(bc == 8){
                bc = 0;
                line = *(++p);
            }
            if(line & (0x80>>bc))
                LCD_Pixel(w, h,WHITE);
            else
                LCD_Pixel(w, h,BLACK);
        }
    }
    return font->w + font->spacing;
}


void SEVEN_Double(uint16_t x, uint16_t y, double val){

}

void test(void *ptr){
uint8_t x = 0;
    font = (font_t *)ptr;

    for (int i = '0'; i < '9' + 1; i++)
    {
        x += drawChar(x, 0, i);
        LCD_Update();
        /* code */
    }   
}

void TEXT_Init(void){

    
    LCD_Init();
    LCD_ClrArea(0, 0, LCD_W, LCD_H);
    LCD_Update();

    test(&defaultFont);
    DelayMs(1000);
    test(&font_seven_seg);
    DelayMs(1000);
    test(&defaultBoldFont);
    DelayMs(1000);
    test(&lcdFont);
    DelayMs(1000);
    test(&pixelDustFont);
    
}
   

void SEVEN_PrintAmps(double amps){
    SEVEN_Double(0,1,amps);
}

void SEVEN_PrintVolts(double volts){
    SEVEN_Double(65,1,volts);
}


void TEXT_print(uint16_t x, uint16_t y, const char* format, ...){
char line[MAX_LINE_CHARS];
va_list argptr;
    va_start(argptr, format);
    sprintf(line, format, argptr);    
    va_end(argptr);
}

