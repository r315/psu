#ifndef _model_psu_h_
#define _model_psu_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "psu.h"

#define MODEL_FLAG_APP              (1 << 0)    // Application updated data
#define MODEL_FLAG_PRESET           (1 << 1)    // Preset value changed
#define MODEL_FLAG_PRESET_IDX       (1 << 2)    // Preset index changed

class ModelPsu: public BUIModel{
public:
    ModelPsu();
    void tick();
    void update();
    void init(void);

    uint8_t hasData();
    uint32_t getOutVoltage(void);
    uint32_t getOutCurrent(void);
    uint32_t getLoadVoltage(void);
    uint32_t getLoadCurrent(void);
    uint32_t getUsbCurrent(void);

    uint32_t getOutVoltagePreset(void){return _out_preset.v;}
    void setOutVoltagePreset(uint32_t v);
    uint32_t getOutCurrentPreset(void){return _out_preset.i;}
    void setOutCurrentPreset(uint32_t i);
    
    uint8_t toggleOutputEnable(void);
    uint8_t getOutputEnable(void);

    preset_t getOutPreset(void){ return _out_preset;}
    uint8_t getPresetIdx(void);
    preset_t getPreset(uint8_t idx);
    void setOutPreset(preset_t pre);
    void setOutPresetIdx(uint8_t idx);

    void updateOutputVoltage(void);
    void updateOutputCurrent(void);
    void updateLoadVoltage(void);
    void updateLoadCurrent(void);
    void updateUsbCurrent(void);

    uint8_t isFlagSet(uint8_t flag){return !!(this->_flags & flag);}
private:
    uint32_t _out_voltage;
    uint32_t _out_current;
    uint32_t _load_voltage;
    uint32_t _load_current;
    uint32_t _usb_current;

    preset_t _out_preset;
    uint8_t _preset_idx;

    uint8_t _flags;
    SemaphoreHandle_t access_data;
};

#ifdef __cplusplus
}
#endif

#endif // _model_psu_h_
