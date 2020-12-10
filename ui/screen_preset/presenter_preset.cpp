
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
            _model->selectPresetByIdx(_view->getSelected());
            _model->applyPsuPreset();
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

buievt_e PresenterPreset::eventHandler(buikeyevt_t *evt){   

    if(_state == PRE_EXIT){
        return BUI_EVT_SEL_SCR(0); //select screen psu
    }

    if(evt->type != BUTTON_PRESSED){
        return BUI_EVT_NONE;
    }

    switch(evt->key){
        case BUTTON_PRE:
            return  BUI_EVT_SEL_SCR(0);            

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
    return BUI_EVT_NONE;
}

void PresenterPreset::moveSelect(int8_t dir){
    int8_t sel;

    sel = (_view->getSelected() + dir) % MAX_PRESETS;

    if(sel < 0){
        sel = MAX_PRESETS + sel;
    }

    _view->select(sel);
}

