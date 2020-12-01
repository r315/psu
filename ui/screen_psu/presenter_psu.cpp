
#include "bui.h"
#include "view_psu.h"
#include "model_psu.h"
#include "presenter_psu.h"

void PresenterPsu::init(void){
    if(_view == NULL){
        _view = new ViewPsu();
    }
    _view->init();
    //assert(_view != NULL);
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

uint8_t PresenterPsu::eventHandler(buievt_t *evt){

    // user input will end here
    if(evt->type != BUTTON_PRESSED){
        return 0;
    }

    switch(_state){
        case PSU_ENABLED:
            stateEnabled(evt);
            break;

        case PSU_IDLE:
            if(evt->key == BUTTON_MODE){
                
                return 1;
            }
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
    return 0;
}

void PresenterPsu::stateIdle(buievt_t *evt){
    switch(evt->key){
        case BUTTON_MODE:
            // switch screen
            _view->suspend();
            _state = PSU_INIT;
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
            // this call is not thread safe
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