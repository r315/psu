
#include "bui.h"
#include "view_psu.h"
#include "model_psu.h"
#include "presenter_psu.h"

void PresenterPsu::notify(void){

    switch(_state){
        case PSU_INIT:
            _state = PSU_ENTER_IDLE;
            break;

        case PSU_IDLE:
            break;

        case PSU_ENABLED:
            uint32_t p,v,i;
            v = _model->getOutVoltage();
            i = _model->getOutCurrent();
            p = v * i;
            _view->updateVoltage(v);
            _view->updateCurrent(i);
            _view->updatePower(p);
            _view->updateGraph();
            break;
        
        case PSU_ENTER_IDLE:
            _view->updateVoltage(_model->getOutVoltagePreset());
            _view->updateCurrent(_model->getOutCurrentPreset());
            _view->updatePower(-1);
            _state = PSU_IDLE;
            break;

        default:
            break;
    }
}

void PresenterPsu::eventHandler(buievt_t *evt){

    // user input will end here
    if(evt->type != BUTTON_PRESSED){
        return;
    }

    switch(evt->key){
        case BUTTON_SET:
            
            break;

        case BUTTON_OUT:
            _state = _model->toggleOutputEnable() ? PSU_ENTER_IDLE : PSU_ENABLED;
            break;

        default:
            break;
    }
}