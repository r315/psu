#ifndef _view_preset_h_
#define _view_preset_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "psu.h"
#include "bui.h"
#include "wi_icon.h"

class ViewPreset : public BUIView{
public:
    ViewPreset();
    ~ViewPreset(){}
    void init(void);
    void draw(void);
    void select(uint8_t idx);
    uint8_t getSelected(void);
    void showPreset(preset_t pre);
private:
    BUIicon _wi_ico[MAX_PRESETS];
    BUIText _wi_pre_v;
    BUIText _wi_pre_i;
    uint8_t _selected;
};

#ifdef __cplusplus
}
#endif

#endif /* _view_preset_h_ */