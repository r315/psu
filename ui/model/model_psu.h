#ifndef _model_psu_h_
#define _model_psu_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "psu.h"

class ModelPsu: public BUIModel{
public:
    ModelPsu();
    void tick();
    void update();
    void init(void);

    // Called from presenter with bui task context to get/set model data
    uint32_t getOutVoltage(void);
    uint32_t getOutCurrent(void);
    uint32_t getLoadVoltage(void);
    uint32_t getLoadCurrent(void);
    uint32_t getUsbCurrent(void);
    uint32_t getCellVoltage(uint8_t c);
    uint32_t getOutVoltagePreset(void);
    uint32_t getOutCurrentPreset(void);
    preset_t getPsuPreset(void);
    preset_t getChargerPreset(void);
    uint8_t getPresetIdx(void);
    preset_t getPreset(uint8_t idx);
    uint8_t getBatteryType(void){return _bt_ty;}
    uint32_t getChargeCurrent(void);
    uint32_t getLoadVoltagePreset(void);
    uint32_t getLoadCurrentPreset(void);
    void setOutVoltagePreset(uint32_t v);
    void setOutCurrentPreset(uint32_t i);
    void setOutPreset(preset_t pre);
    void setPresetIdx(uint8_t idx);
    void setBatteryTypePreset(uint8_t t);
    void setChargerCurrentPreset(uint32_t i);
    void setLoadVoltagePreset(uint32_t mv);
    void setLoadCurrentPreset(uint32_t ma);
    
    // Resource not shared, called from presenter
    uint8_t getOutputEnable(void);
    uint8_t toggleOutputEnable(void);

    /**
     * Called from presenter with app task context,
     * to update model data with data from app
     * */
    void updateOutputVoltage(void);
    void updateOutputCurrent(void);
    void updateLoadVoltage(void);
    void updateLoadCurrent(void);
    void updateUsbCurrent(void);
    void updateCellVoltage(uint8_t c);
    
    // Apply model data to app
    void applyPsuPreset(void);
    void applyChargerPreset(void);
    void applyLoadCurrent(void);
    void disableLoad(void);
    void applyPsuPresetFromIdx();
private:
    uint32_t _out_voltage;
    uint32_t _out_current;
    uint32_t _load_voltage;
    uint32_t _load_current;
    uint32_t _usb_current;
    uint32_t _vb[4];
    preset_t *_preset_list;
    preset_t *_psu_preset;
    preset_t _chg_preset;
    preset_t _load_preset;
    uint8_t _bt_ty;         // Current selected battery type [1, 4]    

    SemaphoreHandle_t access_data;
};

#ifdef __cplusplus
}
#endif

#endif // _model_psu_h_
