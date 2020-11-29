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
    _out_preset = app_getCurrentPreset();
    _preset_idx = app_getCurrentPresetIdx();
}

/**
 * called from bui task after every frame
 * */
void ModelPsu::tick(void){
    if(xSemaphoreTake(access_data, 0) == pdPASS){
        // Thread safe call
        _presenter->update();
        xSemaphoreGive(access_data);
    }
}

/**
 * Called from application task
 * */
void ModelPsu::update(){
    if(xSemaphoreTake(access_data, 0) == pdPASS){
        // Obtain data
        _out_voltage = psu_getOutputVoltage();
        _out_current = psu_getOutputCurrent();
        _load_voltage = psu_getLoadVoltage();
        _load_current = psu_getLoadCurrent();
        _usb_current = psu_getUsbCurrent();
        // Notify for new data
        _flags |= MODEL_FLAG_APP;

        // Check if preset value has changed
        if(_flags & MODEL_FLAG_PRESET){
            app_setPreset(_out_preset);
            _flags &= ~MODEL_FLAG_PRESET;
        }

        // Check if preset index has changed
        if(_flags & MODEL_FLAG_PRESET_IDX){
            // update local preset
            _out_preset = app_getPreset(_preset_idx);
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

uint8_t ModelPsu::toggleOutputEnable(void){
    return app_toggleOutputEnable();
}
uint8_t ModelPsu::getOutputEnable(void){
    return app_isOutputEnabled();
}

uint8_t ModelPsu::getPresetIdx(void){    
    return _preset_idx;
}
preset_t ModelPsu::getPreset(uint8_t idx){
    return app_getPreset(idx);
}
void ModelPsu::setOutPreset(preset_t pre){
    _out_preset = pre;
    _flags |= MODEL_FLAG_PRESET;
}
void ModelPsu::setOutVoltagePreset(uint32_t v){
    _out_preset.v = v; 
    _flags |= MODEL_FLAG_PRESET;
}
void ModelPsu::setOutCurrentPreset(uint32_t i){
    _out_preset.i = i; 
    _flags |= MODEL_FLAG_PRESET;
}
void ModelPsu::setOutPresetIdx(uint8_t idx){
    _preset_idx = idx;
    _flags |= MODEL_FLAG_PRESET_IDX;
}