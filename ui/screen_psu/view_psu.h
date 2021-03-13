#ifndef _view_psu_h_
#define _view_psu_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "bui.h"
#include "wi_dro.h"
#include "wi_icon.h"

class ViewPsu : public BUIView{
public:
    ViewPsu();
    void init(void);
    void draw(void);
    void updateVoltage(uint32_t mv);
    void updateCurrent(uint32_t ma);
    void updateUsbCurrent(uint32_t ma);
    void updatePower(int32_t mw);
    void updateGraph(void);
    void editVoltage(uint8_t dig);
    void editCurrent(uint8_t dig);
    void changeVoltage(uint8_t d);
    void changeCurrent(uint8_t d);
    void showOutIcon(uint8_t v);
    uint32_t getVoltage(void){ return _wi_voltage.getValue();}
    uint32_t getCurrent(void){ return _wi_current.getValue(); }
private:
    BUIText _wi_power;
    BUIdro _wi_current;
    BUIdro _wi_voltage;
    BUIGraph _wi_graph;
    BUIicon  _wi_out_icon;
    BUIText _wi_usb_current;
};

#ifdef __cplusplus
}
#endif

#endif /* _scr_psu_h_ */