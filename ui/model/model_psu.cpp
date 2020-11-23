#include "psu.h"
#include "bui.h"
#include "model_psu.h"

void ModelPsu::update(){
    if(_presenter != NULL){
        _presenter->notify();
    }
}

uint32_t ModelPsu::getOutVoltage(void){
    return psu_getOutputVoltage();
}
uint32_t ModelPsu::getOutCurrent(void){
    return 0;
}
uint32_t ModelPsu::getLoadVoltage(void){
    return 0;
}
uint32_t ModelPsu::getLoadCurrent(void){
    return 0;
}
uint32_t ModelPsu::getInputVoltage(void){
    return 0;
}
uint32_t ModelPsu::getUsbCurrent(void){
    return 0;
}

uint8_t ModelPsu::toggleOutputEnable(void){
    return app_toggleOutputEnable();
}

uint32_t ModelPsu::getOutVoltagePreset(void){
    return _out_current_preset;
}

uint32_t ModelPsu::getOutCurrentPreset(void){
    return _out_current_preset;
}