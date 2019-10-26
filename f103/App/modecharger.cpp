#include "psu.h"


void ModeCharger::redraw(void){
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
    TEXT_drawGfx(90,0, (uint8_t*)&icon_load[0]);
    TEXT_setFont(&pixelDustFont);
    TEXT_print(0,0,"V1 0.0V");
    TEXT_print(0,7,"V2 0.0V");
    TEXT_print(0,14,"V3 0.0V");
    TEXT_print(0,21,"V4 0.0V");    
}

void ModeCharger::modeSet(){

    
}

void ModeCharger::process(State *st){

    
}