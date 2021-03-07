#include "psu.h"
#include "bui.h"
#include "model_psu.h"

const uint32_t batVoltages[4] = {
    4100,
    8300,
    12500,
    16730
};

ModelPsu::ModelPsu(){
    access_data = xSemaphoreCreateBinary();

    if(access_data == NULL){
        DBG_PRINT("Model: Fail to create semaphore\n");
    }

    if(xSemaphoreGive(access_data) != pdPASS){
        DBG_PRINT("Model: WTF? Fail to give created semaphore\n");
    }
}

void ModelPsu::init(void){

    _preset_list = app_getPresetList();
    _preset_idx = app_getSavedPresetIdx();
    _psu_preset = &_preset_list[_preset_idx];
    
    _bt_ty = 1; // 1S
    _chg_preset.v = MAX_CELL_VOLTAGE;
    _chg_preset.i = 100;

    _load_preset.v = 0;     // End voltage,
    _load_preset.i = 200;
}

/**
 * called from bui task after every frame
 * */
void ModelPsu::tick(void){
    if(xSemaphoreTake(access_data, 0) == pdPASS){
        // Thread safe call
        if(_presenter != NULL)
            _presenter->tick();
        xSemaphoreGive(access_data);
    }
}

/**
 * Called from application task
 * */
void ModelPsu::update(){
    if(xSemaphoreTake(access_data, 0) == pdPASS){
        // Update model variables through presenter
        if(_presenter != NULL)
            _presenter->update();
        xSemaphoreGive(access_data);
    }
}

/**
 * @brief App forwarding
 * */
uint8_t ModelPsu::toggleOutputEnable(void){
    return app_toggleOutputEnable();
}
uint8_t ModelPsu::getOutputEnable(void){
    return app_isOutputEnabled();
}

/**
 * @brief Read model data
 * */
uint32_t ModelPsu::getOutVoltage(void){
    return _out_voltage;
}
uint32_t ModelPsu::getOutCurrent(void){
    return _out_current;
}
uint32_t ModelPsu::getLoadVoltage(void){
    return _load_voltage;
}
uint32_t ModelPsu::getLoadCurrent(void){
    return _load_current;
}
uint32_t ModelPsu::getUsbCurrent(void){
    return _usb_current;
}
uint32_t ModelPsu::getCellVoltage(uint8_t c){
    return _vb[c];
}
uint32_t ModelPsu::getOutVoltagePreset(void){
    return _psu_preset->v;
}
uint32_t ModelPsu::getOutCurrentPreset(void){
    return _psu_preset->i;
}
preset_t ModelPsu::getPsuPreset(void){
    return *_psu_preset;
}
uint8_t ModelPsu::getPresetIdx(void){    
    return _preset_idx;
}
preset_t ModelPsu::getPreset(uint8_t idx){
    return app_getPresetList()[idx];
}
preset_t ModelPsu::getChargerPreset(void){
    return _chg_preset;
}
uint32_t ModelPsu::getChargeCurrent(void){
    return _chg_preset.i;
}
uint32_t ModelPsu::getLoadVoltagePreset(void){
    return _load_preset.v;
}
uint32_t ModelPsu::getLoadCurrentPreset(void){
    return _load_preset.i;
}

/**
 * @brief Write new data to model data
 * */
void ModelPsu::setOutPreset(preset_t pre){
    _psu_preset->v = pre.v;
    _psu_preset->i = pre.i;
}
void ModelPsu::setOutVoltagePreset(uint32_t v){
    _psu_preset->v = v; 
}
void ModelPsu::setOutCurrentPreset(uint32_t i){
    _psu_preset->i = i; 
}
void ModelPsu::selectPresetByIdx(uint8_t idx){
    _preset_idx = idx;
    _psu_preset = _preset_list + _preset_idx;
}
void ModelPsu::setChargerCurrentPreset(uint32_t i){
    _chg_preset.i = i;    
}
void ModelPsu::setBatteryTypePreset(uint8_t t){
     _bt_ty = t;
     _chg_preset.v = batVoltages[_bt_ty - 1];
}
void ModelPsu::setLoadCurrentPreset(uint32_t ma){
    _load_preset.i = ma;
}
void ModelPsu::setLoadVoltagePreset(uint32_t mv){
    _load_preset.v = mv;
}

/**
 * @brief Update internal data model with latest data from app,
 * called from presenter class
 * */ 
void ModelPsu::updateOutputVoltage(void){
    _out_voltage = psu_getOutputVoltage();
}
void ModelPsu::updateOutputCurrent(void){
    _out_current = psu_getOutputCurrent();
}
void ModelPsu::updateLoadVoltage(void){
    _load_voltage = psu_getLoadVoltage();
}
void ModelPsu::updateLoadCurrent(void){
    _load_current = psu_getLoadCurrent();
}
void ModelPsu::updateUsbCurrent(void){
    _usb_current = psu_getUsbCurrent();
}
void ModelPsu::updateCellVoltage(uint8_t c){
    _vb[c] = psu_getCellVoltage(c);
}

/**
 * @brief Instruct app to use model data presets
 * */
void ModelPsu::applyPsuPreset(void){
    app_applyPreset(_psu_preset);
}
void ModelPsu::applyChargerPreset(void){
    app_applyPreset(&_chg_preset);
}
void ModelPsu::applyLoadCurrent(void){
    psu_setLoadCurrent(_load_preset.i);
}
void ModelPsu::disableLoad(void){
    psu_setLoadCurrent(0);
}
