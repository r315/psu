#include "bui.h"
#include "presenter_charger.h"

void PresenterCharger::init(void){
    if(_view == NULL){
        _view = new ViewCharger();
    }
    _view->init();
}

void PresenterCharger::tick(void){
    switch(_state){
        case CHG_INIT:

        case CHG_ENTER_IDLE:
            _ncell = _model->getBatteryType();
            _view->showChargingIcon(false);
            _view->updateCurrent(_model->getChargeCurrent());
            _view->updateBatteryType(_ncell);
            _view->updateCapacity(-1);
            _state = CHG_IDLE;
            break;

        case CHG_ENTER_CHARGING:
        {
            _model->setBatteryType(_view->getBatteryType());
            _model->applyChargerPreset();
            _view->showChargingIcon(true);            
            _state = CHG_CHARGING;
            _capacity = 0;
            _ticks = GetTicks();
            break;
        }

        case CHG_CHARGING:
        {
            uint32_t ma = _model->getOutCurrent();
            computeCapacity(ma);
            _view->updateCurrent(ma);
            _view->updateCapacity(_capacity);
        }
        case CHG_IDLE:
        {
            uint32_t pack_voltage = 0;
            for(uint8_t i = 0; i < CHG_MAX_CELL; i++){
                if(i < _ncell){
                    uint32_t cell_voltage = _model->getCellVoltage(i) - pack_voltage;
                    _view->updateCellVoltage(i, cell_voltage);
                    pack_voltage += cell_voltage;
                }else{
                    _view->updateCellVoltage(i, -1);
                }
            }
            // Use calculated voltage,because in idle the relay disconnects both terminals
            _view->updatePackVoltage(pack_voltage);
            break;
        }

        case CHG_END_SET_I:
            _model->setChargerCurrent(_view->getCurrent());
        case CHG_ABORT_SET_I:
            _view->editCurrent(false);
            _state = CHG_ENTER_IDLE;
            break;

        case CHG_END_SET_TYPE:
            _model->setBatteryType(_view->getBatteryType());    /* save battery type on model, this also sets pack voltage on charger preset */ 
            _ncell = _view->getBatteryType();

        case CHG_ABORT_SET_TYPE:
            _view->editBatteryType(false);
            _state = CHG_ENTER_IDLE;
            break;

        default:
            break;
    }
}

void PresenterCharger::update(void){
    for(uint8_t i = 0; i < _ncell; i++){
        _model->updateCellVoltage(i);
    }
    _model->updateOutputCurrent();
}

buievt_e PresenterCharger::eventHandler(buikeyevt_t *evt){
    // user input will end here
    if(evt->type != BUTTON_PRESSED){
        return BUI_EVT_NONE;
    }

    switch(_state){
        case CHG_IDLE:
            if(evt->key == BUTTON_MODE){
                _state = CHG_INIT;
                return BUI_EVT_CHG_SCR;
            }
            stateIdle(evt);
            break;

        case CHG_SET_I:
            stateSetI(evt);
            break;

        case CHG_CHARGING:
            stateCharging(evt);
            break;

        case CHG_SET_TYPE:
            stateSetType(evt);
            break;
            
        default:
            break;
    }

    return BUI_EVT_NONE;
}


void PresenterCharger::stateIdle(buikeyevt_t *evt){
    switch(evt->key){

        case BUTTON_DOWN:
            evt->key = BUTTON_EMPTY;
            stateSetI(evt);
            break;

        case BUTTON_UP:
            evt->key = BUTTON_EMPTY;
            stateSetType(evt);
            break;

        case BUTTON_OUT:
            _state = _model->toggleOutputEnable() ? CHG_ENTER_CHARGING : CHG_ENTER_IDLE;
            break;

        default:
            break;
    }
}

void PresenterCharger::stateCharging(buikeyevt_t *evt){
    switch(evt->key){
        case BUTTON_OUT:
            _state = _model->toggleOutputEnable() ? CHG_ENTER_CHARGING : CHG_ENTER_IDLE;
            break;

        default:
            break;
    }
}
void PresenterCharger::stateSetI(buikeyevt_t *evt){
    switch(evt->key){

        case BUTTON_UP: _view->changeCurrent(1); break;
            
        case BUTTON_DOWN: _view->changeCurrent(-1); break;

        case BUTTON_LEFT: _view->editCurrent(1); break;
            
        case BUTTON_RIGHT: _view->editCurrent(-1); break;

        case BUTTON_SET: 
            _state = CHG_END_SET_I;
            break;

        case BUTTON_MODE:            
            _state = CHG_ABORT_SET_I;
            break;

        case BUTTON_EMPTY:
            _view->editCurrent(true);
            _state = CHG_SET_I;
            break;

        default:
            break;
    }
}

void PresenterCharger::stateSetType(buikeyevt_t *evt){
    switch(evt->key){

        case BUTTON_LEFT:
            _view->scrollBatteryType(-1);
            break;
            
        case BUTTON_RIGHT:
            _view->scrollBatteryType(1);
            break;

        case BUTTON_SET:
            _state = CHG_END_SET_TYPE;
            break;
            
        case BUTTON_MODE:
            _state = CHG_ABORT_SET_TYPE;
            break;

        case BUTTON_EMPTY:
            _view->editBatteryType(true);
            _state = CHG_SET_TYPE;
            break;

        default:
            break;
    }
}

void PresenterCharger::computeCapacity(uint32_t ma){
    uint32_t elapsed = ElapsedTicks(_ticks); 

    // Value will update every 30s
    if(elapsed >= 30000){
        _capacity += ma/(3600/30);
        _ticks = GetTicks();
    }

}