#include "psu.h"
#include "bui.h"
#include "model_psu.h"

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
    _psu_preset = app_getCurrentPreset();
    _preset_idx = app_getCurrentPresetIdx();
    _bt_ty = 0; // 1S
}

/**
 * called from bui task after every frame
 * */
void ModelPsu::tick(void){
    if(xSemaphoreTake(access_data, 0) == pdPASS){
        // Thread safe call
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
        // Notify for new data
        _flags |= MODEL_FLAG_APP;

        // Check if preset value has been edited by user
        if(_flags & MODEL_FLAG_PRESET){
            app_setPreset(_psu_preset);
            _flags &= ~MODEL_FLAG_PRESET;
        }

        // Check if preset selection has changed by user
        if(_flags & MODEL_FLAG_PRESET_IDX){
            // update local preset
            _psu_preset = app_getPreset(_preset_idx);
            // Set new preset
            app_setPresetByIdx(_preset_idx);
            _flags &= ~MODEL_FLAG_PRESET_IDX;
        }

        xSemaphoreGive(access_data);
    }
}

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
    return _vb[c - 1];
}
uint32_t ModelPsu::getOutVoltagePreset(void){
     return _psu_preset.v;
}
uint32_t ModelPsu::getOutCurrentPreset(void){
    return _psu_preset.i;
}
preset_t ModelPsu::getPsuPreset(void){
    return _psu_preset;
}
uint8_t ModelPsu::getPresetIdx(void){    
    return _preset_idx;
}
preset_t ModelPsu::getPreset(uint8_t idx){
    return app_getPreset(idx);
}
preset_t ModelPsu::getChargerPreset(void){
    return _chg_preset;
}

uint8_t ModelPsu::toggleOutputEnable(void){
    return app_toggleOutputEnable();
}
uint8_t ModelPsu::getOutputEnable(void){
    return app_isOutputEnabled();
}

void ModelPsu::setOutPreset(preset_t pre){
    _psu_preset = pre;
    setFlag(MODEL_FLAG_PRESET);
}
void ModelPsu::setOutVoltagePreset(uint32_t v){
    _psu_preset.v = v; 
    setFlag(MODEL_FLAG_PRESET);
}
void ModelPsu::setOutCurrentPreset(uint32_t i){
    _psu_preset.i = i; 
    setFlag(MODEL_FLAG_PRESET);
}
void ModelPsu::setOutPresetIdx(uint8_t idx){
    _preset_idx = idx;
    setFlag(MODEL_FLAG_PRESET_IDX);
}

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
    _vb[c - 1] = psu_getCellVoltage(c);
}
