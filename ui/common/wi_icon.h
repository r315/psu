#ifndef _wi_icon_h_
#define _wi_icon_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "bui.h"

class BUIicon : public BUIWidget{
public:
    BUIicon():BUIWidget(0,0){}
    BUIicon(uint16_t x, uint16_t y, const uint8_t *ico_data);
    void draw(void);
    void setPal(const uint16_t *pal){ _pal = pal;}
    uint8_t isSelected(void){ return isFlagSet(BUI_FLAG_SELECTED); }
    void setVisible(uint8_t v){ if(v) setFlag(BUI_FLAG_VISIBLE); else clrFlag(BUI_FLAG_VISIBLE);}
    void select(uint8_t sel){ if(sel) setFlag(BUI_FLAG_SELECTED); else clrFlag(BUI_FLAG_SELECTED); setInvalid(true);}
private:
    const uint16_t *_pal;
    const uint8_t *_ico_data;
};

#ifdef __cplusplus
}
#endif

#endif /* _scr_icon_h_ */