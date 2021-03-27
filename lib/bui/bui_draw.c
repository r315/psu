
#include "board.h"
#include "draw.h"

#define SCRATCH_BUF_SIZE    512

static volatile uint8_t lcd_busy = 0;
static uint16_t _color;

// use dual buffer for speed
static uint16_t _scratch[2][SCRATCH_BUF_SIZE];
static uint16_t *scratch = _scratch[0];
static uint8_t bufidx = 0;

/**
 * Called from DMA end of transfer interrupt
 * */
void spi_eot(void){
    LCD_CS1;
    lcd_busy = 0;
}

/**
 * @brief Wait for any draw operation to conclude
 * */
void DRAW_WaitOpEnd(void){
    while(lcd_busy);    
}

/**
 * @brief Draws buffer in to the lcd, blocking if the last transfer has not finished yet.
 * Other access lcd access should be avoided.
 * 
 * @param x : x position
 * @param y : y position
 * @param data : data buffer
 * @param w : area width
 * @param h : area height
 */
static void DRAW_BufferLL(uint16_t x, uint16_t y, uint16_t *data, uint16_t w, uint16_t h){
    while(lcd_busy);
    lcd_busy = 1;
    LCD_Window(x, y, w, h);
    LCD_CS0;
    SPI_WriteDMA(data, w * h);
    // swap buffer    
    scratch = _scratch[(++bufidx) & 1];
}

/**
 * @brief draw filled rectangle
 * */
void DRAW_FillRect(uint16_t x, uint16_t y,  uint16_t w, uint16_t h, uint16_t color){
    while(lcd_busy);
    lcd_busy = 1;
    LCD_Window(x, y, w, h);
    LCD_CS0;
    _color = color;
    // Configure transfer to send same color (w*h) times
	SPI_WriteDMA(&_color, (w * h) | 0x80000000);
}

/**
 * @brief draw vertical line
 * 
 * @param x,y : start x,y position
 * @param h : line hight
 * @param color : line color
 * */
void DRAW_VLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color){
    DRAW_FillRect(x, y, 1, h, color);
}

/**
 * @brief draw horizontal line
 * 
 * @param x,y : start x,y position
 * @param h : line width
 * @param color : line color
 * */
void DRAW_HLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color){
    DRAW_FillRect(x, y, w, 1, color);
}

/**
 * @brief 
 * 
 * @param x,y : start x,y position
 * @param h : line width
 * @param color : line color
 * */
void DRAW_Pixel(uint16_t x, uint16_t y, uint16_t color){
    while(lcd_busy);
    LCD_Pixel(x, y, color);
}

/**
 * @brief Draws bitmap on display, data has format {w,h,data...}
 * 
 * @param x : x position
 * @param y : y position
 * @param data : bitmap data
 */
void DRAW_Bitmap(uint16_t x, uint16_t y, uint16_t *data){
    DRAW_BufferLL(x, y, data+2, data[0], data[1]);
}

/**
 * @brief Converts an 1bpp icon to single 16bit color bitmap.
 * Note: icon data should be left aligned
 * 
 * @param bitmap : pointer to bitmap output
 * @param icon : input icon, data format {w,h,data...}
 * @param fcolor : forecolor 16bit color
 * @param bcolor : backgroundcolor 16bit color
 * */
void DRAW_Icon2Bitmap(uint16_t *bitmap, const uint8_t *icon, const uint16_t *pal){
    uint16_t w, h;

    w = *icon++;
    h = *icon++;

    *bitmap++ = w;
    *bitmap++ = h;

    for(uint8_t i = 0; i < h; i++, icon++){
        uint8_t bmap = *icon;
        uint8_t mask = 0x80;
        for(uint8_t j = 0; j < w; j++, mask >>= 1, bitmap++){
            if(mask == 0){
                mask = 0x80;
                bmap = *(++icon);
            }
            if(bmap & mask) *bitmap = pal[1];
            else *bitmap = pal[0];
        }
    }
}

/**
 * @brief Draws 1bpp icon with BLACK background
 * 
 * @param x : x position on display
 * @param y : y position on display
 * @param icon : input icon, data format {w,h,data...}
 * @param color : forecolor
 * */
void DRAW_Icon(uint16_t x, uint16_t y, const uint8_t *ico, const uint16_t *pal){
    DRAW_Icon2Bitmap(scratch, ico, pal);
    DRAW_Bitmap(x, y, scratch);
}

/**
 * @brief Draws character bitmap into buffer
 * 
 * \param buffer : pointer to destination tile buffer
 * \param c : Character to print
 * \param fnt : font to be used
 * \param pal : color palette
 * */
static void blitChar(uint16_t *buffer, uint8_t c, font_t *fnt, const uint16_t *pal){
    const uint8_t *pd;
    
    c -= fnt->offset;
    pd = fnt->data + (c * fnt->h * fnt->bpl);

    // Check out of bound character
    if(pd > fnt->data + fnt->data_len)
        return;

    for(uint16_t h = 0; h < fnt->h; h++, pd++){
        uint8_t bmap = *pd;
        uint8_t mask = 0x80;

        for(uint16_t w = 0; w < fnt->w; w++, mask >>= 1, buffer++){
            if(mask == 0){
                mask = 0x80;
                bmap = *(++pd);
            }
            *buffer = pal[!!(bmap & mask)];
        }
    }
}

/**
 * draws a character using a font table.
 * the font table must be 1bpp
 */
/**
 * @brief Draws a character into the display
 * 
 * \param x : x coordinate
 * \param y : y coordinate
 * \param c : Character to print
 * \param fnt : font to be used
 * \param pal : color palette
 * \return : x coordinate for next character
 * */
uint16_t DRAW_Char(uint16_t x, uint16_t y, uint8_t c, font_t *fnt, const uint16_t *pal){
    //Check if character fits temporary buffer
    if(fnt->w * fnt->h > SCRATCH_BUF_SIZE){
        return 0;
    }

    blitChar(scratch, c, fnt, pal);
    DRAW_BufferLL(x, y, scratch, fnt->w, fnt->h);
    
    return x + fnt->w + fnt->spacing;
}

/**
 * */
uint16_t DRAW_Text(uint16_t x, uint16_t y, const char* str, font_t *fnt, const uint16_t *pal){
    while(*str){
        x = DRAW_Char(x, y, *str++, fnt, pal);
    }
    return x;
}
