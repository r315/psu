#ifndef _model_psu_h_
#define _model_psu_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "psu.h"

class ModelPsu: public BUIModel{
    uint32_t _out_voltage_preset;
    uint32_t _out_current_preset;
public:
    void update();
    void setOutPreset(preset_t pre){
        _out_voltage_preset = pre.v;
        _out_current_preset = pre.i;
    }

    preset_t getOutPreset(void){ return (preset_t){_out_voltage_preset, _out_current_preset};}
    uint32_t getOutVoltage(void);
    uint32_t getOutCurrent(void);
    uint32_t getLoadVoltage(void);
    uint32_t getLoadCurrent(void);
    uint32_t getInputVoltage(void);
    uint32_t getUsbCurrent(void);
    uint32_t getOutVoltagePreset(void){return _out_voltage_preset;}
    void setOutVoltagePreset(uint32_t v){_out_voltage_preset = v;}
    uint32_t getOutCurrentPreset(void){return _out_current_preset;}
    void setOutCurrentPreset(uint32_t i){_out_current_preset = i;}
    
    uint8_t toggleOutputEnable(void);
    uint8_t getOutputEnable(void);
private:
};

#ifdef __cplusplus
}
#endif

#endif // _model_psu_h_
