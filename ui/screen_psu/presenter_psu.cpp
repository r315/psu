
#include "bui.h"
#include "view_psu.h"
#include "model_psu.h"
#include "presenter_psu.h"

void PresenterPsu::init(void){
    // Allocate view
    if(_view == NULL){
        _view = new ViewPsu();
    }    
    // Set state fo be initialized
    _state = PSU_INIT;
}

// Called at the end of frame by bui task
void PresenterPsu::tick(void){

    switch(_state){
        case PSU_INIT:
            if(_model->getOutputEnable()){
                _view->showOutIcon(true);
                _state = PSU_ENABLED;
            }else{
                _view->showOutIcon(false);
                _state = PSU_ENTER_IDLE;
            }
            _view->init();
            break;

        case PSU_IDLE:
            // Check if status has changed
            if(_model->getOutputEnable()){
                _view->showOutIcon(true);
                _state = PSU_ENABLED;
            }
            break;       
        
        case PSU_ENTER_IDLE:
            _view->updateVoltage(_model->getOutVoltagePreset());
            _view->updateCurrent(_model->getOutCurrentPreset());
            _view->updatePower(-1);
            _view->showOutIcon(false);
            _state = PSU_IDLE;
            break;

        case PSU_ENABLED:
        {
            uint16_t v = _model->getOutVoltage();
            uint16_t i = _model->getOutCurrent();
            uint32_t p = (v/1000) * i;
            _view->updateVoltage(v);
            _view->updateCurrent(i);
            _view->updatePower(p);
            _view->updateGraph();
            break;
        }

        case PSU_END_SET_V:
            _model->setOutVoltagePreset(_view->getVoltage());
            _model->applyPsuPreset();
        case PSU_ABORT_SET_V:
            _view->editVoltage(false);
            _state = _model->getOutputEnable() ? PSU_ENABLED : PSU_ENTER_IDLE;
            break;

        case PSU_END_SET_I:
            _model->setOutCurrentPreset(_view->getCurrent());
            _model->applyPsuPreset();
        case PSU_ABORT_SET_I:
            _view->editCurrent(false);
            _state = _model->getOutputEnable() ? PSU_ENABLED : PSU_ENTER_IDLE;
            break;

        
        default:
            break;
    }

    _view->updateUsbCurrent(_model->getUsbCurrent());
}

// Called from app task
void PresenterPsu::update(void){
    // update the necessary model data
    _model->updateOutputVoltage();
    _model->updateOutputCurrent();
    _model->updateUsbCurrent();
}

// Called from bui task at start of frame if any button event has ocurred
buievt_e PresenterPsu::eventHandler(buikeyevt_t *evt){

    // user input will end here
    if(evt->type != BUTTON_PRESSED){
        return BUI_EVT_NONE;
    }

    switch(_state){
        case PSU_ENABLED:
            return stateEnabled(evt);

        case PSU_IDLE:
            return stateIdle(evt);
            
        case PSU_SET_V:
            return stateSetV(evt);

        case PSU_SET_I:
            return stateSetI(evt);

        default:
            break;
    }
    return BUI_EVT_NONE;
}

/**
 * Presenter helper functions
 * */
buievt_e PresenterPsu::stateIdle(buikeyevt_t *evt){
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
            _model->applyPsuPreset();
            _view->showOutIcon(true);
            _state = _model->toggleOutputEnable() ? PSU_ENABLED : PSU_ENTER_IDLE;
            break;

        case BUTTON_MODE:
            return BUI_EVT_SEL_SCR(2);      // select screen load

        case BUTTON_PRE:
            return BUI_EVT_SEL_SCR(1);      // select screen preset

        default:
            break;
    }
    return BUI_EVT_NONE;
}

buievt_e PresenterPsu::stateEnabled(buikeyevt_t *evt){
    switch(evt->key){
        case BUTTON_UP:
            evt->key = BUTTON_EMPTY; // Set empty button to force state change
            stateSetV(evt);
            break;

        case BUTTON_DOWN:
            evt->key = BUTTON_EMPTY;
            stateSetI(evt);
            break;

        case BUTTON_OUT:
            _state = _model->toggleOutputEnable() ? PSU_ENABLED : PSU_ENTER_IDLE;
            break;
        
        case BUTTON_PRE:
            return BUI_EVT_SEL_SCR(1);      // select screen preset
        
        default:
            break;
    }
    return BUI_EVT_NONE;
}

buievt_e PresenterPsu::stateSetV(buikeyevt_t *evt){
    switch(evt->key){        

        case BUTTON_UP: _view->changeVoltage(1); break;
            
        case BUTTON_DOWN: _view->changeVoltage(-1); break;

        case BUTTON_LEFT: _view->editVoltage(1); break;
            
        case BUTTON_RIGHT: _view->editVoltage(-1); break;

        case BUTTON_SET:
            _state = PSU_END_SET_V;
            break;
            
        case BUTTON_MODE:
            _state = PSU_ABORT_SET_V;
            break;

        case BUTTON_EMPTY:
            _view->updateVoltage(_model->getOutVoltagePreset());
            _view->editVoltage(true);
            _state = PSU_SET_V;
            break;

        default:
            break;
    }
    return BUI_EVT_NONE;
}

buievt_e PresenterPsu::stateSetI(buikeyevt_t *evt){
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
            _state = PSU_END_SET_I;
            break;

        case BUTTON_MODE:
            _state = PSU_ABORT_SET_I;
            break;

        case BUTTON_EMPTY:
            _view->updateCurrent(_model->getOutCurrentPreset());
            _view->editCurrent(1);
            _state = PSU_SET_I;
            break;

        default:
            break;
    }
    return BUI_EVT_NONE;
}