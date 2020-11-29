#ifndef _wi_icon_h_
#define _wi_icon_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "bui.h"
#include "wi_icon.h"

class BUIicon : public BUIWidget{
public:
    BUIicon();
    BUIicon(uint16_t x, uint16_t y);
    ~BUIicon(){}
    void draw(void);
    uint8_t isSelected(void){ return isFlagSet(BUI_FLAG_SELECTED); }
    void setPal(const uint16_t *pal){ _pal = pal;}
private:
    const uint16_t *_pal;
};

#ifdef __cplusplus
}
#endif

#endif /* _scr_icon_h_ */