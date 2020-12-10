#include "bui.h"
#include "view_charger.h"

#define V1_POS  0,0
#define V2_POS  0,16
#define V3_POS  0,32
#define V4_POS  0,48
#define CF_POS  96,0
#define MA_POS  96,16
#define AH_POS  96,32
#define PV_POS  96,48

#define CHG_CHARGING_ICON_POS             160-16,3

const uint8_t icon_chr[] = {15,8,
    0x7f, 0xfc,0xc5,0x46,0xdd,0x56,0xdd,0x56,0xdc,0x4e,0xdd,0x56,0xc5,0x56,0x7f,0xfc
};

const char *bt_types[] = {"1S", "2S", "3S", "4S"};

ViewCharger::ViewCharger(){
    _wi_vc = (BUIText**)bui_malloc(sizeof(BUIText*) * CHG_MAX_CELL);
    configASSERT(_wi_vc != NULL );

    for(uint8_t i = 0 ; i < CHG_MAX_CELL; i++){        
        _wi_vc[i] = new BUIText(0, i*16);
        addWidget(_wi_vc[i]);
    }

    _wi_ma = new BUIdro(MA_POS);    
    _wi_ma->init(10, 100, MIN_CURRENT, MAX_CURRENT, currentFormat);
    _wi_ma->setFont(&courierFont);    
    
    _wi_chg_icon = new BUIicon(CHG_CHARGING_ICON_POS,icon_chr);    
    _wi_chg_icon->setPal((const uint16_t[]){BLACK, BLACK, BLUE});

    _wi_bt = new BUIlist(CF_POS);
    _wi_bt->setList(bt_types, sizeof(bt_types)/sizeof(size_t));
    _wi_bt->setPal((const uint16_t[]){BLACK, YELLOW});
    _wi_bt->select(1);

    _wi_ah = new BUIText(AH_POS);

    _wi_pv = new BUIText(PV_POS);

    addWidget(_wi_ma);
    addWidget(_wi_ah);
    addWidget(_wi_bt);
    addWidget(_wi_chg_icon);
    addWidget(_wi_pv);
}

void ViewCharger::init(void){    
    // Invalidate view, this will trigger a call to view->draw()
    setInvalid(true);    
    // Invalidate widgets
    BUIWidget::invalidateList(getWidgets());

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
    formater(_txt_buf, c + 1, v);
    _wi_vc[c]->setText(_txt_buf);
}

void ViewCharger::updateCurrent(uint32_t i){
    _wi_ma->setValue(i);
}
void ViewCharger::updateCapacity(uint32_t ah){
    xsprintf(_txt_buf, "%uAh", ah);
    _wi_ah->setText(_txt_buf);
}

void ViewCharger::showChargingIcon(uint8_t v){
    _wi_chg_icon->setVisible(v);
    _wi_chg_icon->setInvalid(true);
}

void ViewCharger::editCurrent(uint8_t dig){
    _wi_ma->editValue(dig);
}

void ViewCharger::changeCurrent(uint8_t d){
    _wi_ma->changeValue(d);
}

void ViewCharger::editBatteryType(uint8_t e){
    _wi_bt->edit(e);
}

void ViewCharger::updateBatteryType(uint8_t e){
    _wi_bt->select(e - 1);
}

void ViewCharger::scrollBatteryType(int8_t e){
    _wi_bt->scroll(e);
}

/**
 * \return 1:1S, .., 4:4S
 * */
uint8_t ViewCharger::getBatteryType(void){
    return _wi_bt->getSelect() + 1;
}

void ViewCharger::updatePackVoltage(uint32_t pv){
    xsprintf(_txt_buf, "%.2fV", (float)(pv/1000.0));
    _wi_pv->setText(_txt_buf);
}
