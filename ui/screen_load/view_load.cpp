#include "bui.h"
#include "view_load.h"

ViewLoad::ViewLoad(){

}

void ViewLoad::init(void){
    // Invalidate view
    setInvalid(true);
    // Invalidate widgets
    //BUIWidget::invalidateList(getWidgets());
}

void ViewLoad::draw(void){
    if(isInvalid()){
        DRAW_FillRect(0, 0, LCD_W, LCD_H, GREEN);
        setInvalid(false);
    }
}
