
#include "text.h"
#include "board.h" 
#include <stdio.h>
#include <stdarg.h>

#define MAX_TILE_SIZE   (16*16)
static font_t *font;
static uint16_t tile[MAX_TILE_SIZE];

static uint16_t drawDp(uint16_t x, uint16_t y){
    LCD_FillRect(x-1,y, 4, font->h, BLACK);
    LCD_FillRect(x,y + font->h - 2, 2,2, WHITE);
    return 3; 
}

/**
 * draws a character using a font table.
 * the font table must be 1bpp
 */
static uint8_t drawChar(uint16_t x, uint16_t y, uint8_t c){
    const uint8_t *pd;
    
    c -= font->offset;
    pd = font->data + (c * font->h * font->bpl);

    // Check out of bound character
    if(pd > font->data + font->data_len)
        return 0;

    //Check if character fits temporary buffer
    if(font->w * font->h > MAX_TILE_SIZE){
        return 0;
    }

    uint16_t *pt = tile;

    for(uint16_t h = y; h < y +font->h; h++, pd++){
        uint8_t line = *pd;
        for(uint16_t w = x, bc = 0; w < x + font->w; w++, bc++, pt++){
            if(bc == 8){
                bc = 0;
                line = *(++pd);
            }
            if(line & (0x80>>bc))
                *pt = WHITE;
            else
                *pt = BLACK;
        }
    }
    LCD_Window(x, y, font->w, font->h);
    LCD_Write(tile, font->w * font->h);
    return font->w + font->spacing;
}

/**
 * 
 * */
void TEXT_setFont(font_t *fnt){
    font = fnt;
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

/**
 * 
 * */
void TEXT_dro(uint16_t x, uint16_t y, float val, uint8_t precision, int8_t blank_place){
char value[6], *p, i = 0;
    if(precision == 2)
        sprintf(value,"%.2f", val);
    else
        if(val < 10)
            sprintf(value,"0%.1f", val);
        else
            sprintf(value,"%.1f", val);

    p = value;
    while(*p){
        if(*p == '.'){
            x += drawDp(x,y);
        }else{
            if((i++) == blank_place)
                x += drawChar(x,y, '9' + 1); // Space for font_seven_seg
            else
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

void TEXT_Init(void){
    LCD_FillRect(0, 0, LCD_W, LCD_H, BLACK);
}

void TEXT_print(uint16_t x, uint16_t y, const char* str){    
    while(*str){
        x += drawChar(x, y, *str++);
    }    
}
