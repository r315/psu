#ifndef _view_psu_h_
#define _view_psu_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "bui.h"
#include "wi_dro.h"

class ViewPsu : public BUIView{
public:
    ViewPsu();
    ~ViewPsu();
    void updateVoltage(uint32_t mv);
    void updateCurrent(uint32_t ma);
    void updatePower(int32_t mw);
    void updateGraph(void);
    void editVoltage(uint8_t dig);
    void editCurrent(uint8_t dig);
private:
    BUIText _wi_power;
    BUIdro _wi_current;
    BUIdro _wi_voltage;
    BUIGraph _wi_graph;    
};

#ifdef __cplusplus
}
#endif

#endif /* _scr_psu_h_ */