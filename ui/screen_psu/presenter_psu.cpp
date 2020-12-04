
#include "bui.h"
#include "view_psu.h"
#include "model_psu.h"
#include "presenter_psu.h"

void PresenterPsu::init(void){
    if(_view == NULL){
        _view = new ViewPsu();
    }
    _view->init();
    _state = PSU_INIT;
}

void PresenterPsu::destroy(void){
    if(_view != NULL){
        //delete _view;
        //_view = NULL;
    }
}

void PresenterPsu::tick(void){

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
        {
            if(_model->isFlagSet(MODEL_FLAG_APP)){
                uint16_t v = _model->getOutVoltage();
                uint16_t i = _model->getOutCurrent();
                uint32_t p = (v/1000) * i;
                _view->updateVoltage(v);
                _view->updateCurrent(i);
                _view->updatePower(p);
                _view->updateGraph();
            }
            break;
        }
        
        default:
            break;
    }
}

void PresenterPsu::update(void){
    // update model data
    _model->updateOutputVoltage();
    _model->updateOutputCurrent();
}

buievt_e PresenterPsu::eventHandler(buikeyevt_t *evt){

    // user input will end here
    if(evt->type != BUTTON_PRESSED){
        return BUI_EVT_NONE;
    }

    switch(_state){
        case PSU_ENABLED:
        case PSU_IDLE:
            if(evt->key == BUTTON_MODE){
                return BUI_EVT_CHG_SCR;
            }else if(evt->key == BUTTON_PRE){
                return (buievt_e)(BUI_EVT_SEL_SCR + 1);
            }
            if(_state == PSU_IDLE){
                stateIdle(evt); 
            }else{
                stateEnabled(evt);
            }
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
    return BUI_EVT_NONE;
}

void PresenterPsu::stateIdle(buikeyevt_t *evt){
    switch(evt->key){
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

void PresenterPsu::stateEnabled(buikeyevt_t *evt){
    switch(evt->key){
        case BUTTON_MODE:
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

void PresenterPsu::stateSetV(buikeyevt_t *evt){
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
            // TODO: fix, this call is not thread safe
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

void PresenterPsu::stateSetI(buikeyevt_t *evt){
    switch(evt->key){

        case BUTTON_UP:
            _view->changeCurrent(1);
            break;
            
        case BUTTON_DOWN:
            _view->changeCurrent(-1);
            break;

        case BUTTON_LEFT:
            _view->editCurrent(1);
            break;
            
        case BUTTON_RIGHT:
            _view->editCurrent(-1);
            break;

        case BUTTON_SET:
            _model->setOutCurrentPreset(_view->getCurrent());
        case BUTTON_MODE:
            _view->editCurrent(0);
            _state = _model->getOutputEnable() ? PSU_ENABLED : PSU_ENTER_IDLE;
            break;

        case BUTTON_EMPTY:
            _view->updateCurrent(_model->getOutCurrentPreset());
            _view->editCurrent(1);
            _state = PSU_SET_I;
            break;

        default:
            break;
    }
}