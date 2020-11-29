#include "bui.h"
#include "presenter_charger.h"

void PresenterCharger::update(void){

}

void PresenterCharger::eventHandler(buievt_t *evt){
    // user input will end here
    if(evt->type != BUTTON_PRESSED){
        return;
    }

    switch(evt->key){
        case BUTTON_MODE:
            // switch screen
            _view.suspend();
            break;
        default:
            break;
    }
}