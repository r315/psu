
#include "bui.h"
#include "view_preset.h"
#include "model_psu.h"
#include "presenter_preset.h"


void PresenterPreset::update(void){
    switch(_state){
        case INIT:
            _view.select(_model->getPresetIdx());
            _view.showPreset(_model->getPreset(_view.getSelected()));
            _state = IDLE;
            break;

        case IDLE:
            break;       
        
        case EXIT:
            _model->setOutPresetIdx(_view.getSelected());
            _view.suspend();
            _state = INIT;
            break;

        case UPD:
            _view.showPreset(_model->getPreset(_view.getSelected()));
            _state = IDLE;
            break;

        default:
            break;
    }
}

void PresenterPreset::eventHandler(buievt_t *evt){
    // user input will end here
    if(evt->type != BUTTON_PRESSED){
        return;
    }

    _state = UPD;

    switch(evt->key){
        case BUTTON_MODE:
            // switch screen
            _view.suspend();
            _state = IDLE;
            break;

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
            _state = EXIT;
            break;

        default:
            break;
    }
   
}

void PresenterPreset::moveSelect(int8_t dir){
    int8_t sel;

    sel = (_view.getSelected() + dir) % MAX_PRESETS;

    if(sel < 0){
        sel = MAX_PRESETS + sel;
    }

    _view.select(sel);
}

