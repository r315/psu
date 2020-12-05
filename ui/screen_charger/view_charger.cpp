#include "bui.h"
#include "view_charger.h"

#define V1_POS  0,0
#define V2_POS  0,16
#define V3_POS  0,32
#define V4_POS  0,48
#define CF_POS  96,0
#define MA_POS  96,16
#define AH_POS  96,32

#define CHG_CHARGING_ICON_POS             160-16,3

const uint8_t icon_chr[] = {15,8,
    0x7f, 0xfc,0xc5,0x46,0xdd,0x56,0xdd,0x56,0xdc,0x4e,0xdd,0x56,0xc5,0x56,0x7f,0xfc
};


ViewCharger::ViewCharger():
    _wi_ma(MA_POS),
    _wi_ah(AH_POS),
    _wi_cf(CF_POS),
    _wi_chg_icon(CHG_CHARGING_ICON_POS,icon_chr)
{
    _wi_vc = (BUIText**)bui_malloc(sizeof(BUIText*) * CHG_MAX_CELL);
    configASSERT(_wi_vc != NULL );

    for(uint8_t i = 0 ; i < CHG_MAX_CELL; i++){        
        _wi_vc[i] = new BUIText(0, i*16);
        configASSERT(_wi_vc[i] != NULL );
        addWidget(_wi_vc[i]);
    }
    
    _wi_ma.init(10, 100, MIN_CURRENT, MAX_CURRENT, currentFormat);
    _wi_ma.setFont(&courierFont);
    _wi_chg_icon.setPal((const uint16_t[]){BLACK, BLACK, BLUE});

    addWidget(&_wi_ma);
    addWidget(&_wi_ah);
    addWidget(&_wi_cf);
    addWidget(&_wi_chg_icon);
}

void ViewCharger::init(void){
    _wi_ah.setText("0.13Ah");
    _wi_cf.setText("4S");
    _wi_ma.setInvalid(true);
    // Invalidate view, this will trigger a call to view->draw()
    setInvalid(true);
    
    // Invalidate widgets
    //BUIWidget::invalidateList(getWidgets());

    _txt_buf = (char*)bui_malloc(10);
    configASSERT(_txt_buf != NULL);
}

void ViewCharger::draw(void){
    if(isInvalid()){
        DRAW_FillRect(0, 0, LCD_W, LCD_H, BLACK);
        setInvalid(false);
    }
}

static void formater(char *buf, uint8_t i, int32_t v){
    if(v < 0){
        xsprintf(buf, "V%u: -----", i);
    }else{
        xsprintf(buf, "V%u: %.2fV", i, (float)(v/1000.0));
    }
}

void ViewCharger::updateCellVoltage(uint8_t c, int32_t v){
    formater(_txt_buf, c, v);
    _wi_vc[c-1]->setText(_txt_buf);
}

void ViewCharger::updateCurrent(uint32_t i){
    _wi_ma.setValue(i);
}
void ViewCharger::updateCapacity(uint32_t ah){

}
void ViewCharger::updateBatConfiguration(uint8_t s){

}
void ViewCharger::showChargingIcon(uint8_t v){
    _wi_chg_icon.setVisible(v);
    _wi_chg_icon.setInvalid(true);
}

void ViewCharger::editCurrent(uint8_t dig){
    _wi_ma.editValue(dig);
}

void ViewCharger::changeCurrent(uint8_t d){
    _wi_ma.changeValue(d);
}
