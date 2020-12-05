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

#define CHG_MAX_CELL        4

class ViewCharger : public BUIView{
public:
    ViewCharger();
    ~ViewCharger(){}
    void init(void);
    void draw(void);

    void updateCellVoltage(uint8_t c, int32_t v);
    void updateCurrent(uint32_t i);
    void updateCapacity(uint32_t ah);
    void updateBatConfiguration(uint8_t s);
    void showChargingIcon(uint8_t v);
    void editCurrent(uint8_t dig);
    void changeCurrent(uint8_t d);
    uint32_t getCurrent(void){ return _wi_ma.getValue(); }
private:
    BUIText **_wi_vc;
    BUIdro _wi_ma;
    BUIText _wi_ah;
    BUIText _wi_cf;
    BUIicon  _wi_chg_icon;
    char *_txt_buf;
};

#ifdef __cplusplus
}
#endif

#endif /* _view_preset_h_ */