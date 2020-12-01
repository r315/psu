#include "bui.h"
#include "view_charger.h"

#define V1_POS  0,0
#define V2_POS  0,16
#define V3_POS  0,32
#define V4_POS  0,48
#define CF_POS  96,0
#define MA_POS  96,16
#define AH_POS  96,32

ViewCharger::ViewCharger():
    _wi_v1(V1_POS),
    _wi_v2(V2_POS),
    _wi_v3(V3_POS),
    _wi_v4(V4_POS),
    _wi_ma(MA_POS),
    _wi_ah(AH_POS),
    _wi_cf(CF_POS)
{    
    addWidget(&_wi_v1);
    addWidget(&_wi_v2);
    addWidget(&_wi_v3);
    addWidget(&_wi_v4);
    addWidget(&_wi_ma);
    addWidget(&_wi_ah);
    addWidget(&_wi_cf);
}

void ViewCharger::init(void){
    _wi_v1.setText("V1: 1.23V");
    _wi_v2.setText("V2: 1.23V");
    _wi_v3.setText("V3: 1.23V");
    _wi_v4.setText("V4: 1.23V");

    _wi_ma.setText("500mA");
    _wi_ah.setText("0.13Ah");
    _wi_cf.setText("4S");
    // Invalidate view
    setInvalid(true);
    // Invalidate widgets
    //BUIWidget::invalidateList(getWidgets());
}

void ViewCharger::draw(void){
    if(isInvalid()){
        DRAW_FillRect(0, 0, LCD_W, LCD_H, RED);
        setInvalid(false);
    }
}

void ViewCharger::updateV1(uint32_t v){

}
void ViewCharger::updateV2(uint32_t v){

}
void ViewCharger::updateV3(uint32_t v){

}
void ViewCharger::updateV4(uint32_t v){

}
void ViewCharger::updateCurrent(uint32_t i){

}
void ViewCharger::updateCapacity(uint32_t ah){

}
void ViewCharger::updateBatConfiguration(uint8_t s){

}
