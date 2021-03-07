#include "bui.h"
#include "presenter_load.h"
#include "model_psu.h"

void PresenterLoad::init(void){
    if(_view == NULL){
        _view = new ViewLoad();
        _capacity  = 0;
        _elapsed_ticks = 0;
    }
    _view->init();
}

void PresenterLoad::tick(void){
    static uint32_t last_ticks = 0;

     switch(_state){
        case LOAD_INIT:        
            _view->updateCapacity(_capacity);            
            _view->updateTime(_elapsed_ticks/1000);       

        case LOAD_ENTER_IDLE:
            _model->disableLoad();
            _view->updateCurrent(_model->getLoadCurrentPreset());
            _view->updateVoltage(_model->getLoadVoltagePreset());
            _view->updatePower(-1);
            _view->showLoadIcon(false);
            _state = LOAD_IDLE;

        case LOAD_IDLE:        
            break;        
        

        case LOAD_END_SET_I:
            _model->setLoadCurrentPreset(_view->getCC());
        case LOAD_ABORT_SET_I:
            _view->editCC(false);
            _state = LOAD_ENTER_IDLE;
            break;

        case LOAD_END_SET_V:
            _model->setLoadVoltagePreset(_view->getEndVoltage());
        case LOAD_ABORT_SET_V:
            _view->editEndVoltage(false);
            _state = LOAD_ENTER_IDLE;
            break;

        case LOAD_ENABLE_LOAD:
            _model->applyLoadCurrent();
            _view->showLoadIcon(true);
            _state = LOAD_RUNNING;
            last_ticks = GetTicks();

        case LOAD_RUNNING:
        {            
            uint32_t i = _model->getLoadCurrent();
            uint32_t v = _model->getLoadVoltage();
            _view->updateCurrent(i);
            _view->updateVoltage(v);
            _view->updatePower((v/1000) * i);
            _view->updateGraph();

            uint32_t ticks = ElapsedTicks(last_ticks);
            // Value will update every 25s, match graph size
            if(ticks >= 25000){
                _elapsed_ticks += ticks;
                _capacity += i/(3600/25);
                last_ticks = GetTicks();
            }

            _view->updateCapacity(_capacity);
            _view->updateTime(_elapsed_ticks/1000);
            if(v < _model->getLoadVoltagePreset()){
                _state = LOAD_END;
            }            
            break;
        }

        case LOAD_END:
            _state = LOAD_ENTER_IDLE;
            break;

        default:
            break;
     }

}

void PresenterLoad::update(void){
    _model->updateLoadVoltage();
    _model->updateLoadCurrent();
}

buievt_e PresenterLoad::eventHandler(buikeyevt_t *evt){
    // user input will end here
    if(evt->type != BUTTON_PRESSED){
        return BUI_EVT_NONE;
    }

    switch(_state){
        case LOAD_IDLE:
            return stateIdle(evt);

        case LOAD_RUNNING:
            return stateRunning(evt);

        case LOAD_SET_I:
            stateSetI(evt);
            break;

        case LOAD_SET_V:
            stateSetEndVoltage(evt);
            break; 

        default:
            break;
    }

    return BUI_EVT_NONE;
}

buievt_e PresenterLoad::stateIdle(buikeyevt_t *evt){
    switch(evt->key){

        case BUTTON_DOWN:
            evt->key = BUTTON_EMPTY;
            stateSetI(evt);
            break;

        case BUTTON_UP:
            evt->key = BUTTON_EMPTY;
            stateSetEndVoltage(evt);
            break;

        case BUTTON_OUT:
            _state = LOAD_ENABLE_LOAD;
            break;

        case BUTTON_MODE:
            _state = LOAD_INIT;
            return BUI_EVT_CHG_SCR;

         case BUTTON_PRE:
            _elapsed_ticks = 0;
            _capacity = 0;
            _state = LOAD_INIT;
            break;

        default:
            break;
    }
    return BUI_EVT_NONE;
}

buievt_e PresenterLoad::stateRunning(buikeyevt_t *evt){
    switch(evt->key){

        case BUTTON_OUT:
            _state = LOAD_ENTER_IDLE;
            break;       

        default:
            break;
    }
    return BUI_EVT_NONE;
}

void PresenterLoad::stateSetI(buikeyevt_t *evt){
    switch(evt->key){

        case BUTTON_UP: _view->changeCC(1); break;
            
        case BUTTON_DOWN: _view->changeCC(-1); break;

        case BUTTON_LEFT: _view->editCC(1); break;
            
        case BUTTON_RIGHT: _view->editCC(-1); break;

        case BUTTON_SET: 
            _state = LOAD_END_SET_I;
            break;

        case BUTTON_MODE:            
            _state = LOAD_ABORT_SET_I;
            break;
        // Enter CC
        case BUTTON_EMPTY: 
            _view->editCC(true);
            _state = LOAD_SET_I;
            break;

        default:
            break;
    }
}

void PresenterLoad::stateSetEndVoltage(buikeyevt_t *evt){
    switch(evt->key){

        case BUTTON_UP: _view->changeEndVoltage(1); break;
            
        case BUTTON_DOWN: _view->changeEndVoltage(-1); break;

        case BUTTON_LEFT: _view->editEndVoltage(1); break;
            
        case BUTTON_RIGHT: _view->editEndVoltage(-1); break;

        case BUTTON_SET: 
            _state = LOAD_END_SET_V;
            break;

        case BUTTON_MODE:            
            _state = LOAD_ABORT_SET_V;
            break;
        // Enter CC
        case BUTTON_EMPTY: 
            _view->editEndVoltage(true);
            _state = LOAD_SET_V;
            break;

        default:
            break;
    }
}