
#include "bui.h"
#include "view_preset.h"
#include "model_psu.h"
#include "presenter_preset.h"

void PresenterPreset::init(void){
    if(_view == NULL){
        _view = new ViewPreset();
    }
    _view->init();
    _state = PRE_INIT;
}

void PresenterPreset::destroy(void){
    if(_view != NULL){
        //delete _view;
        //_view = NULL;
    }
}

void PresenterPreset::tick(void){
    switch(_state){
        case PRE_INIT:
            _view->select(_model->getPresetIdx());
            _view->showPreset(_model->getPreset(_view->getSelected()));
            _state = PRE_IDLE;
            break;

        case PRE_IDLE:
            break;       
        
        case PRE_SET:
            _model->setOutPresetIdx(_view->getSelected());
            _state = PRE_EXIT;
            break;

        case PRE_CHG:
            _view->showPreset(_model->getPreset(_view->getSelected()));
            _state = PRE_IDLE;
            break;
        
        default:
            break;
    }
}

void PresenterPreset::update(void){
    
}

uint8_t PresenterPreset::eventHandler(buievt_t *evt){   

    if(_state == PRE_EXIT){
        return 1;
    }

    if(evt->type != BUTTON_PRESSED){
        return 0;
    }

    switch(evt->key){
        case BUTTON_MODE:
            return 1;            

        case BUTTON_UP:
            moveSelect((MAX_PRESETS>>1));
            _state = PRE_CHG;
            break;

        case BUTTON_DOWN:
            moveSelect((MAX_PRESETS>>1));
            _state = PRE_CHG;
            break;

        case BUTTON_LEFT:
            moveSelect(-1);
            _state = PRE_CHG;
            break;
        
        case BUTTON_RIGHT:
            moveSelect(1);
            _state = PRE_CHG;
            break;

        case BUTTON_SET:
            _state = PRE_SET;
            break;

        default:
            break;
    }
    return 0;
}

void PresenterPreset::moveSelect(int8_t dir){
    int8_t sel;

    sel = (_view->getSelected() + dir) % MAX_PRESETS;

    if(sel < 0){
        sel = MAX_PRESETS + sel;
    }

    _view->select(sel);
}

