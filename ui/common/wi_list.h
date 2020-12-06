#ifndef _wi_list_h_
#define _wi_list_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "bui.h"

class BUIlist : public BUIText{
public:
    BUIlist(){BUIlist(0,0);}
    BUIlist(uint16_t x, uint16_t y);
    void draw(void);
    uint8_t isSelected(void){ return isFlagSet(BUI_FLAG_SELECTED); }
    void setVisible(uint8_t v){ if(v) setFlag(BUI_FLAG_VISIBLE); else clrFlag(BUI_FLAG_VISIBLE);}
    void select(uint8_t sel);
    void scroll(int8_t s);
    void edit(uint8_t e);
    void setList(const char **list, uint8_t size);
    uint8_t getSelect(void);
private:
    const char **_list_text;
    uint8_t _selected;
    uint8_t _time;
    uint8_t _list_size;
};

#ifdef __cplusplus
}
#endif

#endif /* _wi_list_h_ */