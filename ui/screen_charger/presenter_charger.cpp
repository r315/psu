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

void PresenterCharger::update(void){

}

buievt_e PresenterCharger::eventHandler(buikeyevt_t *evt){
    // user input will end here
    if(evt->type != BUTTON_PRESSED){
        return BUI_EVT_NONE;
    }

    switch(evt->key){
        case BUTTON_MODE:
            // switch screen
            //_view.suspend();
            return BUI_EVT_CHG_SCR;
            break;
        default:
            break;
    }

    return BUI_EVT_NONE;
}