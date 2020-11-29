
#include <string.h>
#include "bui.h"
#include "wi_icon.h"

BUIicon::BUIicon() : BUIWidget(0,0){

}

BUIicon::BUIicon(uint16_t x = 0, uint16_t y = 0) : BUIWidget(x ,y){
    
}


void BUIicon::draw(void){
    if(isInvalid()){
        if(isVisible()){
            if(isSelected()){
            //TEXT_SetPalette((const uint16_t []){BLACK, YELLOW});
            //xsprintf(gOut, "%.1fV ", _presets[idx].v/1000.0f);
            //TEXT_Print(PRESET_INFO1_POS, gOut);
            //xsprintf(gOut, "%.2fA ", _presets[idx].i/1000.0f);
            //TEXT_Print(PRESET_INFO2_POS, gOut);
            //memset16(scratch, PRESET_SELECT, PRESET_SIZE);
            //TEXT_SetPalette((const uint16_t []){PRESET_SELECT, WHITE});
                DRAW_FillRect(_x, _y, 16, 16, _pal[1]);
            }else{
                //memset16(scratch, PRESET_BCOLOR, PRESET_SIZE);
                //TEXT_SetPalette((const uint16_t []){PRESET_BCOLOR, WHITE});
                DRAW_FillRect(_x, _y, 16, 16, _pal[0]);
            }
        }
        setInvalid(false);
    }
}