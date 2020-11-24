
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
        
        case PSU_ENTER_IDLE:
            _view->updateVoltage(_model->getOutVoltagePreset());
            _view->updateCurrent(_model->getOutCurrentPreset());
            _view->updatePower(-1);
            _state = PSU_IDLE;
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

        default:
            break;
    }
}

void PresenterPsu::eventHandler(buievt_t *evt){

    // user input will end here
    if(evt->type != BUTTON_PRESSED){
        return;
    }

    switch(_state){
        case PSU_ENABLED:
            stateEnabled(evt);
            break;

        case PSU_IDLE:
            stateIdle(evt);
            break;

        case PSU_SET_V:
            stateSetV(evt);
            break;

        case PSU_SET_I:
            stateSetI(evt);
            break;

        default:
            break;
    }
}

void PresenterPsu::stateIdle(buievt_t *evt){
    switch(evt->key){
        case BUTTON_MODE:
            // switch screen
            break;

        case BUTTON_UP:
            evt->key = BUTTON_EMPTY;
            stateSetV(evt);
            break;

        case BUTTON_DOWN:
            evt->key = BUTTON_EMPTY;
            stateSetI(evt);
            break;

        case BUTTON_OUT:
            _state = _model->toggleOutputEnable() ? PSU_ENABLED : PSU_ENTER_IDLE;
            break;

        default:
            break;
    }
}

void PresenterPsu::stateEnabled(buievt_t *evt){
    switch(evt->key){
        case BUTTON_MODE:
            // switch screen
            break;

        case BUTTON_UP:
            evt->key = BUTTON_EMPTY;
            stateSetV(evt);
            break;

        case BUTTON_DOWN:
            evt->key = BUTTON_EMPTY;
            stateSetI(evt);
            break;

        case BUTTON_OUT:
            _state = _model->toggleOutputEnable() ? PSU_ENABLED : PSU_ENTER_IDLE;
            break;

        default:
            break;
    }
}

void PresenterPsu::stateSetV(buievt_t *evt){
    switch(evt->key){        

        case BUTTON_UP:
            _view->changeVoltage(1);
            break;
            
        case BUTTON_DOWN:
            _view->changeVoltage(-1);
            break;

        case BUTTON_LEFT:
            _view->editVoltage(1);
            break;
            
        case BUTTON_RIGHT:
            _view->editVoltage(-1);
            break;

        case BUTTON_SET:
            _model->setOutVoltagePreset(_view->getVoltage());
        case BUTTON_MODE:
            _view->editVoltage(0);
            _state = _model->getOutputEnable() ? PSU_ENABLED : PSU_ENTER_IDLE;
            break;

        case BUTTON_EMPTY:
            _view->updateVoltage(_model->getOutVoltagePreset());
            _view->editVoltage(1);
            _state = PSU_SET_V;
            break;

        default:
            break;
    }
}

void PresenterPsu::stateSetI(buievt_t *evt){

}