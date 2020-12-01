#include "bui.h"
#include "presenter_load.h"

void PresenterLoad::init(void){
    if(_view == NULL){
        _view = new ViewLoad();
    }
    _view->init();
    //assert(_view != NULL);
}

void PresenterLoad::destroy(void){
    if(_view != NULL){
        //delete _view;
        //_view = NULL;
    }
}

void PresenterLoad::tick(void){

}

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