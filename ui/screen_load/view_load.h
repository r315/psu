#ifndef _view_load_h_
#define _view_load_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "bui.h"
#include "wi_dro.h"
#include "wi_icon.h"

class ViewLoad : public BUIView{
public:
    ViewLoad();
    void init(void);
    void draw(void);
    void updateCapacity(int32_t ma);
    void updateVoltage(int32_t v);
    void updateTime(uint32_t ts);
    void updatePower(int32_t p);
    void updateCurrent(uint32_t ma);
    void updateGraph(void);
    void showLoadIcon(uint8_t v);
private:
    BUIdro *_wi_ma;
    BUIText *_wi_ah;
    BUIText *_wi_time;
    BUIdro *_wi_voltage;
    BUIText *_wi_pwr;
    BUIGraph *_wi_graph;
    BUIicon  *_wi_load_icon;
    char *_txt_buf;
};

#ifdef __cplusplus
}
#endif

#endif /* _view_preset_h_ */