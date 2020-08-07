
#include "text.h"
#include "board.h" 
#include <stdio.h>
#include <stdarg.h>
#include "draw.h"


static font_t *gfont;
static const uint16_t *gpal;

/**
 * draws a character using a font table.
 * the font table must be 1bpp
 */
uint8_t TEXT_Char(uint16_t x, uint16_t y, uint8_t c){
    //Check if character fits temporary buffer
    if(gfont->w * gfont->h > SCRATCH_BUF_SIZE){
        return 0;
    }
    TEXT_TileChar(scratch, c);
    DRAW_Tile(x, y, scratch, gfont->w, gfont->h);
    return gfont->w + gfont->spacing;
}

/**
 * @brief Draws a char into a tile using global palette
 * 
 * @param tile : pointer to destination tile buffer
 * @param c : Character to print
 * */
void TEXT_TileChar(uint16_t *tile, uint8_t c){
    const uint8_t *pd;
    
    c -= gfont->offset;
    pd = gfont->data + (c * gfont->h * gfont->bpl);

    // Check out of bound character
    if(pd > gfont->data + gfont->data_len)
        return;

    for(uint16_t h = 0; h < gfont->h; h++, pd++){
        uint8_t bmap = *pd;
        uint8_t mask = 0x80;

        for(uint16_t w = 0; w < gfont->w; w++, mask >>= 1, tile++){
            if(mask == 0){
                mask = 0x80;
                bmap = *(++pd);
            }
            *tile = gpal[!!(bmap & mask)];
        }
    }
}

/**
 * */
void TEXT_Print(uint16_t x, uint16_t y, const char* str){    
    while(*str){
        x += TEXT_Char(x, y, *str++);
    }    
}

/**
 * 
 * */
void TEXT_SetFont(font_t *fnt){
    gfont = fnt;
}

/**
 * */
void TEXT_SetPalette(const uint16_t *palette){
    gpal = palette;
}

/**
 * */
void TEXT_Init(void){
    TEXT_SetPalette((const uint16_t []){BLACK, WHITE});
    TEXT_SetFont(DEFAULT_FONT);
    DRAW_FillRect(0, 0, LCD_W, LCD_H, BLACK);
}
/**
 * 
 * */
#if 0
void TEXT_dro(uint16_t x, uint16_t y, float value, uint8_t precision, int8_t blank_place){
char out[6], *p, i = 0;

    if(precision == 2)
        sprintf(out,"%.2f", value);
    else
        if(value < 10)
            sprintf(out,"0%.1f", value);
        else
            sprintf(out,"%.1f", value);

    p = out;
    while(*p){
        if((i++) == blank_place)
            x += drawChar(x,y, ' ');
        else
            x += drawChar(x,y, *p);        
        p++;
    }
}
#elif 0
void TEXT_dro(uint16_t x, uint16_t y, double val, uint8_t places){
long int_part, frac_part;
char prec;
  
    int_part = (long)val;
		
	frac_part = 0;
	prec = 0;
	while ((prec++) < places){
        val *= 10;
        frac_part = (frac_part * 10) + (long)val - ((long)val / 10) * 10;  // + ((long)f%10);			
	}

    TEXT_SetFont(&font_seven_seg);
    x += drawChar(x,y, '0' + int_part/10);
    x += drawChar(x,y, '0' + int_part%10);
    x += drawDp(x,y);
    x += drawChar(x+3,y, '0' + frac_part/10);
    if(places == 2)
        drawChar(x,y, '0' + int_part%10);
}
#endif


