#include "bui.h"
#include "view_charger.h"

ViewCharger::ViewCharger():
    _wi_v1(0,0),
    _wi_v2(0,16)
    //_wi_v3(0,32),
    //_wi_v4(0,48)
{
    
    addWidget(&_wi_v1);
    addWidget(&_wi_v2);
    //addWidget(&_wi_v3);
    //addWidget(&_wi_v4);   
}

void ViewCharger::init(void){


    _wi_v1.setText("1.23V");
    _wi_v2.setText("1.23V");
    //_wi_v3.setText("1.23V");
    //_wi_v4.setText("1.23V");
    // Invalidate view
    setInvalid(true);
    // Invalidate widgets
    //BUIWidget::invalidateList(getWidgets());
}

void ViewCharger::draw(void){
    if(isInvalid()){
        DRAW_FillRect(0, 0, LCD_W, LCD_H, RED);
        setInvalid(false);
    }
}
