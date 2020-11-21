#ifndef _scr_psu_h_
#define _scr_psu_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "bui.h"


class SCRpsu : public BUIScreen{
public:
    SCRpsu();
    ~SCRpsu();
    void updateVoltage(uint32_t mv);
    void updateCurrent(uint32_t ma);
    void updatePower(uint32_t mw);
    void keyPressed(uint32_t key);
    void keyReleased(uint32_t key);
private:
    BUIText wi_power;
    BUIText wi_current;
    BUIText wi_voltage;
    BUIGraph wi_graph;
    char *_txt_buffer;
};

#ifdef __cplusplus
}
#endif

#endif /* _scr_psu_h_ */