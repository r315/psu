

#include <stdio.h>
#include "psu.h"
#include "draw.h"
#include "font.h"

#define PRESET_POS          16,16
#define PRESET_SPACING      16
#define PRESET_BCOLOR       CYAN
#define PRESET_FCOLOR       WHITE
#define PRESET_SELECT       PINK
#define PRESET_SIZE         (16*16)
#define PRESET_TEXT_FONT    &courierFont
//#define PRESET_TEXT_FONT    &defaultFont

void ScreenPreset::selectPreset(uint8_t idx){
    if(idx == _selected){
        TEXT_SetPalette((const uint16_t []){BLACK, YELLOW});
        xsprintf(gOut, "%.1fV ", _presets[idx].v);
        TEXT_Print(108, 10, gOut);
        xsprintf(gOut, "%.2fA ", _presets[idx].i);
        TEXT_Print(108, 30, gOut);
        memset16(scratch, PRESET_SELECT, PRESET_SIZE);
        TEXT_SetPalette((const uint16_t []){PRESET_SELECT, WHITE});
    }else{
        memset16(scratch, PRESET_BCOLOR, PRESET_SIZE);
        TEXT_SetPalette((const uint16_t []){PRESET_BCOLOR, WHITE});
    }

    uint8_t x = idx % (MAX_PRESETS >> 1);
    uint8_t y = (idx - x) / (MAX_PRESETS >> 1);

    DRAW_Tile(x*32 + 16, y*32 + 16, scratch, 16, 16);
    TEXT_Char(x*32 + 16 + 4, y * 32 + 16 + 1, '1' + idx);
}

void ScreenPreset::redraw(void){   
    for(uint8_t i = 0; i < MAX_PRESETS; i++){ 
        selectPreset(i);
    }
}

void ScreenPreset::moveSelect(int8_t dir){
    _selected = (_selected + dir) % MAX_PRESETS;
    if(_selected < 0){
        _selected = MAX_PRESETS + _selected;
    }
}

void ScreenPreset::process(){
    if(BUTTON_GetEvents() == BUTTON_PRESSED){        
        switch(BUTTON_VALUE){
            
            case BUTTON_UP: 
                moveSelect((MAX_PRESETS>>1));
                break;

            case BUTTON_DOWN: 
                moveSelect((MAX_PRESETS>>1));
                break;

            case BUTTON_LEFT:
                moveSelect(-1);
                break;

            case BUTTON_RIGHT: 
                moveSelect(1);
                break;

            case BUTTON_SET:
                app_setPreset(&_presets[_selected]);
                app_selectMode(0); // screen psu
                return;
        }

        redraw();
    }
}

void ScreenPreset::init(){
    DRAW_FillRect(0, 0, LCD_W, LCD_H, BLACK);
    TEXT_SetFont(PRESET_TEXT_FONT);

    _presets = app_getPresetList();

    //redraw(); // performed on process
}