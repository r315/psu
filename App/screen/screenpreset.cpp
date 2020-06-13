

#include <stdio.h>
#include "psu.h"
#include "draw.h"

#define PRESET_POS          16,16
#define PRESET_SPACING      16
#define PRESET_BCOLOR       CYAN
#define PRESET_FCOLOR       WHITE
#define PRESET_SELECT       PINK
#define PRESET_SIZE         (16*16)
#define PRESET_TEXT_FONT    &courierFont


void ScreenPreset::createPreset(uint16_t idx, uint16_t *buf){
char out[20];    
    if(idx == selected){
        sprintf(out, "%.1fV, %.2fA", presets[idx].voltage, presets[idx].current);
        TEXT_Print(0, 80 - 14, out);
        memset16(buf, PRESET_SELECT, PRESET_SIZE); 
    }else{
        memset16(buf, PRESET_BCOLOR, PRESET_SIZE);
    }

    uint16_t x = idx % 3;
    uint16_t y = (idx - x) / 3;

    DRAW_Tile(x*32 + 16, y*32 + 16, buf, 16, 16);
}

void ScreenPreset::redraw(void){
    DRAW_FillRect(0, 0, LCD_W, LCD_H, BLACK);
    selected = 3;
    TEXT_SetFont(PRESET_TEXT_FONT);

    for(uint8_t i = 0; i < MAX_PRESET; i++){
        presets[i].voltage = i;
        presets[i].current = i;        
        createPreset(i, scratch);
    }
    //DRAW_Icon(PSU_ICON_POS, (uint8_t*)&icon_psu[0], BLUE);
    //DRAW_Icon(VOLTAGE_UNIT_POS, (uint8_t*)&dro_unit_v[0], GREEN);
    //DRAW_Icon(CURRENT_UNIT_POS, (uint8_t*)&dro_unit_a[0], GREEN);
}

void ScreenPreset::process(psu_t *st){

}

void ScreenPreset::modeSet(){

}

void ScreenPreset::init(){
    redraw();
}