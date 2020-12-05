
#include <string.h>
#include "bui.h"
#include "wi_icon.h"

BUIicon::BUIicon(uint16_t x, uint16_t y, const uint8_t *data = NULL) : BUIWidget(x ,y){
    _ico_data = data;
}


void BUIicon::draw(void){
    if(isInvalid()){
        if(isVisible()){
            // TODO: Improve icon selection
            if(isSelected()){
                DRAW_FillRect(_x, _y, _ico_data[0], _ico_data[1], _pal[2]);
            }else{                
                DRAW_Icon(_x, _y, _ico_data, _pal + 1);
            }
        }else{
            DRAW_FillRect(_x, _y, _ico_data[0], _ico_data[1], _pal[0]);
        }
        setInvalid(false);
    }
}