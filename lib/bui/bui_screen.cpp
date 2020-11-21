#include "bui.h"


BUIScreen::BUIScreen(){
    _widget.elem = NULL;  
    _widget.next = NULL;
}

uint8_t BUIScreen::addWidget(BUIWidget *wi){
    uint8_t idx = buiAddNode(&_widget, (void *)wi);
    return idx;
}