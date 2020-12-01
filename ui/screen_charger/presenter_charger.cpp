#include "bui.h"
#include "presenter_charger.h"

void PresenterCharger::update(void){

}

uint8_t PresenterCharger::eventHandler(buievt_t *evt){
    // user input will end here
    if(evt->type != BUTTON_PRESSED){
        return 0;
    }

    switch(evt->key){
        case BUTTON_MODE:
            // switch screen
            //_view.suspend();
            return 1;
            break;
        default:
            break;
    }

    return 0;
}