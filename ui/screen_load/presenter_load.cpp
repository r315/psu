#include "bui.h"
#include "presenter_load.h"

void PresenterLoad::update(void){

}

uint8_t PresenterLoad::eventHandler(buievt_t *evt){
    // user input will end here
    if(evt->type != BUTTON_PRESSED){
        return 0;
    }

    switch(evt->key){
        case BUTTON_MODE:
            // switch screen
            //_view->suspend();
            return 1;
            break;
        default:
            break;
    }

    return 0;
}