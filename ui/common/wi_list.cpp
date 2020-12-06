#include "bui.h"
#include "wi_list.h"
#include <string.h>

#define LIST_BLINK_TIME_MASK             8

BUIlist::BUIlist(uint16_t x, uint16_t y):BUIText(x,y){
    _selected = 0;
}

void BUIlist::edit(uint8_t e){
    if(e == false){        
        clrFlag(BUI_FLAG_EDIT);
        setVisible(true);    
    }else{
        setFlag(BUI_FLAG_EDIT);
    }
    
    setInvalid(true);
}

void BUIlist::setList(const char **list, uint8_t size){
    _list_text = list;
    _list_size = size;
}

void BUIlist::select(uint8_t sel){
    _selected = sel;
    setVisible(true);    
    setInvalid(true);
}

void BUIlist::scroll(int8_t s){

    if(s > 1 || s <-1){
        return;
    }

    int8_t tmp = _selected + s;

    if(tmp < 0){
        tmp = _list_size - 1;
    }else if(tmp >= _list_size){
        tmp = 0;
    }

    _selected = tmp;
    
    setVisible(true);    
    setInvalid(true);
}

uint8_t BUIlist::getSelect(void){
    return _selected;
}

void BUIlist::draw(void){
    if(isFlagSet(BUI_FLAG_EDIT)){
        if((++_time) & LIST_BLINK_TIME_MASK){
            setVisible(true);
        }else{
            setVisible(false);
        }
        setInvalid(true);
    }

    if(isInvalid()){        
        if(isVisible()){            
            setText(_list_text[_selected]);                                    
        }else{
            memset(_text, ' ', _len);
        }
        drawString(_x, _y, _text, _font, _pal);        
        setInvalid(false);
    }    
}