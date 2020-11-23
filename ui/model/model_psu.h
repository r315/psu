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
    void updateOutPreset(preset_t pre){
        _out_voltage_preset = pre.v;
        _out_current_preset = pre.i;
    }
    uint32_t getOutVoltage(void);
    uint32_t getOutCurrent(void);
    uint32_t getLoadVoltage(void);
    uint32_t getLoadCurrent(void);
    uint32_t getInputVoltage(void);
    uint32_t getUsbCurrent(void);
    uint32_t getOutVoltagePreset(void);
    uint32_t getOutCurrentPreset(void);
    uint8_t toggleOutputEnable(void);
private:
};

#ifdef __cplusplus
}
#endif

#endif // _model_psu_h_
