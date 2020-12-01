#ifndef _view_charger_h_
#define _view_charger_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "psu.h"
#include "bui.h"

class ViewCharger : public BUIView{
public:
    ViewCharger();
    ~ViewCharger(){}
    void init(void);
    void draw(void);

    void updateV1(uint32_t v);
    void updateV2(uint32_t v);
    void updateV3(uint32_t v);
    void updateV4(uint32_t v);
    void updateCurrent(uint32_t i);
    void updateCapacity(uint32_t ah);
    void updateBatConfiguration(uint8_t s);

private:
    BUIText _wi_v1;
    BUIText _wi_v2;
    BUIText _wi_v3;
    BUIText _wi_v4;
    BUIText _wi_ma;
    BUIText _wi_ah;
    BUIText _wi_cf;
};

#ifdef __cplusplus
}
#endif

#endif /* _view_preset_h_ */