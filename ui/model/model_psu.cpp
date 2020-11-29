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

/**
 * called from bui task after every frame
 * */
void ModelPsu::tick(void){
    if(xSemaphoreTake(access_data, 0) == pdPASS){
        // check if app updated data
        if(_flags & MODEL_FLAG_APP){
            _presenter->update();
            _flags &= ~MODEL_FLAG_APP;
        }
        // call presenter methods
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

        // Check if data to app has changed
        // and update
        if(_flags & MODEL_FLAG_PRE){
            app_setPreset(_out_preset);
            _flags &= ~MODEL_FLAG_PRE;
        }
        // Clear flags
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