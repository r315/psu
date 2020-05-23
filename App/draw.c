
#include "board.h"
#include "draw.h"

uint16_t scratch[SCRATCH_BUF_SIZE];

/**
 * @brief Draws bitmap on display, data has format {w,h,data...}
 * 
 * @param x : x position
 * @param y : y position
 * @param data : bitmap data
 */
void DRAW_Bitmap(uint16_t x, uint16_t y, uint16_t *data){
    uint16_t w = data[0], h = data[1];    
    LCD_Window(x, y, w, h);
    LCD_Write(&data[2], w * h);   
}

/**
 * @brief Draws tile
 * 
 * @param x : x position
 * @param y : y position
 * @param data : tile data
 * @param w : tile width
 * @param h : tile height
 */
void DRAW_Tile(uint16_t x, uint16_t y, uint16_t *data, uint16_t w, uint16_t h){
    LCD_Window(x, y, w, h);
    LCD_Write(data, w * h);   
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
void DRAW_Icon2Bitmap(uint16_t *bitmap, uint8_t *icon, uint16_t fcolor, uint16_t bcolor){
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
            if(bmap & mask) *bitmap = fcolor;
            else *bitmap = bcolor;
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
void DRAW_Icon(uint16_t x, uint16_t y, uint8_t *ico, uint16_t color){
    DRAW_Icon2Bitmap(scratch, ico, color, BLACK);
    DRAW_Bitmap(x, y, scratch);
}

/**
 * @brief Draws 1bpp icon
 * 
 * @param x : x position on display
 * @param y : y position on display
 * @param icon : input icon, data format {w,h,data...}
 * @param fcolor : forecolor
 * @param bcolor : backgroundcolor
 * */
void DRAW_Icon2(uint16_t x, uint16_t y, uint8_t *ico, uint16_t fcolor, uint16_t bcolor){
    DRAW_Icon2Bitmap(scratch, ico, fcolor, bcolor);
    DRAW_Bitmap(x, y, scratch);
}