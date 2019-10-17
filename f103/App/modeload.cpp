#include "psu.h"


void ModeLoad::redraw(void){
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
    TEXT_drawGfx(LOAD_ICON_POS, (uint8_t*)&icon_load[0]);
}

void ModeLoad::modeSet(){

}

void ModeLoad::process(State *st){
    
}