
#include "bui.h"
#include "view_preset.h"
#include "model_psu.h"
#include "presenter_preset.h"


void PresenterPreset::update(void){
    switch(_state){
        case PRE_INIT:
            _view->select(_model->getPresetIdx());
            _view->showPreset(_model->getPreset(_view->getSelected()));
            _state = PRE_IDLE;
            break;

        case PRE_IDLE:
            break;       
        
        case PRE_EXIT:
            _model->setOutPresetIdx(_view->getSelected());
            _view->suspend();
            _state = PRE_INIT;
            break;

        case PRE_UPD:
            _view->showPreset(_model->getPreset(_view->getSelected()));
            _state = PRE_IDLE;
            break;

        default:
            break;
    }
}

uint8_t PresenterPreset::eventHandler(buievt_t *evt){
    // user input will end here
    if(evt->type != BUTTON_PRESSED){
        return 0;
    }

    _state = PRE_UPD;

    switch(evt->key){
        case BUTTON_MODE:
            // switch screen
            //_view->suspend();
            _state = PRE_INIT;
            return 1;
            

        case BUTTON_UP:
            moveSelect((MAX_PRESETS>>1));
            break;

        case BUTTON_DOWN:
            moveSelect((MAX_PRESETS>>1));
            break;

        case BUTTON_LEFT:
            moveSelect(-1);
            break;
        
        case BUTTON_RIGHT:
            moveSelect(1);
            break;

        case BUTTON_SET:
            _state = PRE_EXIT;
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

