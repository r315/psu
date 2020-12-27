#include "bui.h"
#include "presenter_load.h"
#include "model_psu.h"

void PresenterLoad::init(void){
    if(_view == NULL){
        _view = new ViewLoad();
    }
    _view->init();
    //assert(_view != NULL);
}

void PresenterLoad::tick(void){
     switch(_state){
        case LOAD_INIT:        
            _view->updateCapacity(0);
            _view->init();
            _view->updateTime(0);

        case LOAD_ENTER_IDLE:
            _model->disableLoad();
            _view->updateCurrent(_model->getLoadCurrentPreset());
            _view->updateVoltage(_model->getLoadVoltagePreset());
            _view->updatePower(-1);
            _view->showLoadIcon(false);
            _state = LOAD_IDLE;

        case LOAD_IDLE:
        {
            break;
        }

        case LOAD_ENABLE_LOAD:
            _model->applyLoadCurrent();
            _view->showLoadIcon(true);
            _state = LOAD_RUNNING;


        case LOAD_RUNNING:
        {
            uint32_t i = _model->getLoadCurrent();
            uint32_t v = _model->getLoadVoltage();
            _view->updateCurrent(i);
            _view->updateVoltage(v);
            _view->updatePower((v/1000) * i);
            _view->updateGraph();
            computeCapacity(i);
            _view->updateCapacity(_capacity);
            _view->updateTime(_elapsed_ticks/1000);
            break;
        }

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

        default:
            break;
    }

    return BUI_EVT_NONE;
}


buievt_e PresenterLoad::stateIdle(buikeyevt_t *evt){
    switch(evt->key){

        case BUTTON_DOWN:
            //evt->key = BUTTON_EMPTY;
            //stateSetI(evt);
            break;

        case BUTTON_UP:
            //evt->key = BUTTON_EMPTY;
            //stateSetType(evt);
            break;

        case BUTTON_OUT:
            _state = LOAD_ENABLE_LOAD;
            break;

        case BUTTON_MODE:
            _state = LOAD_INIT;
            return BUI_EVT_CHG_SCR;

        default:
            break;
    }
    return BUI_EVT_NONE;
}

buievt_e PresenterLoad::stateRunning(buikeyevt_t *evt){
    switch(evt->key){

        case BUTTON_OUT:
            _state = LOAD_ENTER_IDLE;

        default:
            break;
    }
    return BUI_EVT_NONE;
}

void PresenterLoad::computeCapacity(uint32_t ma){
    uint32_t ticks = ElapsedTicks(_elapsed_ticks); 

    // Value will update every 25s, match graph size
    if(ticks >= 25000){
        _elapsed_ticks += ticks;
        _capacity += ma/(3600/25);
    }
}