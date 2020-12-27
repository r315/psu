#ifndef _view_charger_h_
#define _view_charger_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "psu.h"
#include "bui.h"
#include "wi_icon.h"
#include "wi_dro.h"
#include "wi_list.h"

#define CHG_MAX_CELL        4

class ViewCharger : public BUIView{
public:
    ViewCharger();
    ~ViewCharger(){}
    void init(void);
    void draw(void);

    void updateCellVoltage(uint8_t c, int32_t v);
    void updateCurrent(uint32_t i);
    void updateCapacity(int32_t mah);
    void showChargingIcon(uint8_t v);
    void editCurrent(uint8_t dig);
    void changeCurrent(uint8_t d);
    uint32_t getCurrent(void){ return _wi_ma->getValue(); }
    void editBatteryType(uint8_t e);
    void scrollBatteryType(int8_t e);
    void updateBatteryType(uint8_t e);
    uint8_t getBatteryType(void);
    void updatePackVoltage(uint32_t pv);
private:
    BUIText **_wi_vc;           // Cell voltages
    BUIdro *_wi_ma;             // Charge current
    BUIText *_wi_ah;            // Ah put into battery
    BUIlist *_wi_bt;            // Battery type
    BUIicon  *_wi_chg_icon;     // Charge icon
    BUIText *_wi_pv;            // Pack voltage
    char *_txt_buf;
};

#ifdef __cplusplus
}
#endif

#endif /* _view_preset_h_ */