
#include "text.h"
#include "board.h" 
#include <stdio.h>
#include <stdarg.h>


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
        return 0;

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

/**
 * data format: w,h,data...
 * 
 */
void TEXT_drawGfx(uint16_t x, uint16_t y, uint8_t *data){
    uint8_t *p = data + 2;

    for(uint16_t h = y; h < y + data[1]; h++, p++){
        uint8_t line = *p;
        for(uint16_t w = x, bc = 0; w < x + data[0]; w++, bc++){
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
}

uint16_t drawDp(uint16_t x, uint16_t y){
    LCD_Fill(x-1,y, 4, font->h, BLACK);
    LCD_Fill(x,y + font->h - 2, 2,2, WHITE);
    return 3; 
}

void TEXT_dro(uint16_t x, uint16_t y, double val, uint8_t places){
char value[20], *p;
    if(places == 2)
        sprintf(value,"%.2f", val);
    else
        sprintf(value,"%.1f", val);
    p = value;
    while(*p){
        if(*p == '.'){
            x += drawDp(x,y);
        }else{
            x += drawChar(x,y, *p);
        }
        p++;
    }
}

/**
 * 
 */
void _TEXT_dro(uint16_t x, uint16_t y, double val, uint8_t places){
long int_part, frac_part;
char prec;
  
    int_part = (long)val;
		
	frac_part = 0;
	prec = 0;
	while ((prec++) < places){
        val *= 10;
        frac_part = (frac_part * 10) + (long)val - ((long)val / 10) * 10;  // + ((long)f%10);			
	}

    TEXT_setFont(&font_seven_seg);
    x += drawChar(x,y, '0' + int_part/10);
    x += drawChar(x,y, '0' + int_part%10);
    x += drawDp(x,y);
    x += drawChar(x+3,y, '0' + frac_part/10);
    if(places == 2)
        drawChar(x,y, '0' + int_part%10);
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
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
    LCD_Update();
return;
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
char line[MAX_LINE_CHARS], *p;
va_list argptr;
    va_start(argptr, format);
    sprintf(line, format, argptr);    
    va_end(argptr);

    p = line;
    while(*p){
        x += drawChar(x, y, *p++);
    }    
}

