#include "psu.h"
#include "bui.h"
#include "view_preset.h"
#include "wi_icon.h"

#define PRESET_POS_X                108
#define PRESET_INFO1_POS            PRESET_POS_X, 16
#define PRESET_INFO2_POS            PRESET_POS_X, 32
#define PRESET_UNSELECTED_COLOR       WHITE
#define PRESET_SELECTED_COLOR         CYAN

ViewPreset::ViewPreset(): _wi_pre_v(PRESET_INFO1_POS), _wi_pre_i(PRESET_INFO2_POS){
    for(uint8_t i = 0 ; i < MAX_PRESETS; i++){
        uint8_t x = i % (MAX_PRESETS >> 1);
        uint8_t y = (i - x) / (MAX_PRESETS >> 1);
        _wi_ico[i].setPos(x*32 + 16, y*32 + 16);
        _wi_ico[i].setPal((const uint16_t []){PRESET_UNSELECTED_COLOR, PRESET_SELECTED_COLOR});
        addWidget(&_wi_ico[i]);
    }

    addWidget(&_wi_pre_v);
    addWidget(&_wi_pre_i);
    _wi_pre_v.setPal((const uint16_t []){BLACK, YELLOW});
    _wi_pre_i.setPal((const uint16_t []){BLACK, YELLOW});
    _selected = 0;
}

void ViewPreset::init(void){
    // Invalidate view
    setInvalid(true);
    // Invalidate widgets
    BUIWidget::invalidateList(getWidgets());
}

void ViewPreset::draw(void){
    if(isInvalid()){
        DRAW_FillRect(0, 0, LCD_W, LCD_H, BLACK);
        setInvalid(false);
    }
}

void ViewPreset::select(uint8_t idx){
    _wi_ico[_selected].select(false);
    _selected = idx;
    _wi_ico[_selected].select(true);
}

uint8_t ViewPreset::getSelected(void){
    return _selected;
}

void ViewPreset::showPreset(preset_t pre){
    char out[5];
    xsprintf(out, "%.1fV ", pre.v/1000.0f);
    _wi_pre_v.setText(out);
    xsprintf(out, "%.2fA ", pre.i/1000.0f);
    _wi_pre_i.setText(out);
}