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
            _view->showChargingIcon(false);
            _view->updateCurrent(_model->getOutCurrentPreset());
            _view->updateBatteryType(_model->getBatteryType());
            _view->updateCapacity(0);
            _state = CHG_IDLE;
            break;

        case CHG_ENTER_CHARGING:
            _view->showChargingIcon(true);
            _state = CHG_CHARGING;
            break;

        case CHG_IDLE:
        case CHG_CHARGING:            
            for(uint8_t i = 1; i <= CHG_MAX_CELL; i++){
                if(i < _ncell){
                    _view->updateCellVoltage(i, _model->getCellVoltage(i));
                }else{
                    _view->updateCellVoltage(i, -1);
                }
            }
            break;

        case CHG_END_SET_TYPE:
            _model->setBatteryType(_view->getBatteryType());            

        case CHG_ABORT_SET_TYPE:
            _view->editBatteryType(false);
            _state = CHG_ENTER_IDLE;
            break;

        default:
            break;
    }
}

void PresenterCharger::update(void){
    for(uint8_t i = 1; i <= _ncell; i++){
        _model->updateCellVoltage(i);
    }
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

void PresenterCharger::stateSetI(buikeyevt_t *evt){
    switch(evt->key){

        case BUTTON_UP: _view->changeCurrent(1); break;
            
        case BUTTON_DOWN: _view->changeCurrent(-1); break;

        case BUTTON_LEFT: _view->editCurrent(1); break;
            
        case BUTTON_RIGHT: _view->editCurrent(-1); break;

        case BUTTON_SET: _model->setOutCurrentPreset(_view->getCurrent());            
        case BUTTON_MODE:
            _view->editCurrent(0);
            _state = CHG_ENTER_IDLE;
            break;

        case BUTTON_EMPTY:
            _view->updateCurrent(_model->getOutCurrentPreset());
            _view->editCurrent(1);
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