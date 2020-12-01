#include "bui.h"
#include "presenter_charger.h"

void PresenterCharger::init(void){
    if(_view == NULL){
        _view = new ViewCharger();
    }
    _view->init();
    //assert(_view != NULL);
}

void PresenterCharger::destroy(void){
    if(_view != NULL){
        //delete _view;
        //_view = NULL;
    }
}

void PresenterCharger::tick(void){

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