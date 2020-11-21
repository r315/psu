
#include "bui.h"
#include <string.h>

BUIText::BUIText(uint16_t x = 0, uint16_t y = 0) : BUIWidget(x ,y){
    _font = BUI_DEFAULT_FONT;
    _text = NULL;
    _len = 0;
    _pal = (const uint16_t[]){BLACK, WHITE};
}

BUIText::~BUIText(void){
    if(_text != NULL){
        bui_free(_text);
    }    
}

void BUIText::setText(const char *text){
    if(_text != NULL){
        bui_free(_text);
    }
    
    _len = strlen(text);
    
    _text = (char*)bui_malloc(_len);
    
    if(_text != NULL){
        strcpy(_text, (char *)text);
        setInvalid(true);
    }
}

void BUIText::draw(void){
    if(isInvalid()){
        if(isVisible()){
            drawString(_x,_y, _text, _font, _pal);        
        }else{
            uint16_t x = _x;
            for(uint16_t i = 0; i < _len; i++){
                x = drawChar(x, _y, ' ', _font, _pal);            
            }
        }
        setInvalid(false);
    }
}

void BUIText::setPal(const uint16_t *pal){
    _pal = pal;
}

void BUIText::setFont(font_t *font){
    _font = font;
}
