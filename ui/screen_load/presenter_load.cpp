#include "bui.h"
#include "presenter_load.h"

void PresenterLoad::init(void){
    if(_view == NULL){
        _view = new ViewLoad();
    }
    _view->init();
    //assert(_view != NULL);
}

void PresenterLoad::tick(void){

}

void PresenterLoad::update(void){

}

buievt_e PresenterLoad::eventHandler(buikeyevt_t *evt){
    // user input will end here
    if(evt->type != BUTTON_PRESSED){
        return BUI_EVT_NONE;
    }

    switch(evt->key){
        case BUTTON_MODE:
            // switch screen
            //_view->suspend();
            return BUI_EVT_CHG_SCR;
            break;
        default:
            break;
    }

    return BUI_EVT_NONE;
}