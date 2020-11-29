#include "bui.h"
#include "view_charger.h"

ViewCharger::ViewCharger(){

}

void ViewCharger::init(void){
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
