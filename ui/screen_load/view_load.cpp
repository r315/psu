#include "psu.h"
#include "bui.h"
#include "view_load.h"
#include "wi_dro.h"

#define LOAD_MA_POS         0,0
#define LOAD_ENDV_POS       50,0
#define LOAD_PWR_POS        100,0
#define LOAD_AH_POS         88,16
#define LOAD_TIME_POS       0,16

#define LOAD_GRAPH_WIDTH    144 // => 25s each point, total 3600s
#define LOAD_GRAPH_HIGHT    40
#define LOAD_GRAPH_POS      8,(LCD_H - LOAD_GRAPH_HIGHT - 2)

#define LOAD_ICON_POS       140,16

const uint8_t icon_load[] = {17,8,
    0x7f,0xff,0x00,0xd1,0x1d,0x80,0xd5,0x51,0x80,0xd5,0xd5,0x80,0xd5,0x15,0x80,0xd5,0x55,0x80,0xd1,0x11,0x80,0x7f,0xff,0x00
};

ViewLoad::ViewLoad(){
    
    _wi_ah = new BUIText(LOAD_AH_POS);
    _wi_pwr = new BUIText(LOAD_PWR_POS);
    _wi_time = new BUIText(LOAD_TIME_POS);
    _wi_voltage = new BUIdro(LOAD_ENDV_POS);
    _wi_ma = new BUIdro(LOAD_MA_POS);

    _wi_graph = new BUIGraph(LOAD_GRAPH_POS,
            LOAD_GRAPH_WIDTH, LOAD_GRAPH_HIGHT, 
            2,
            (const uint16_t[]){0x18E3, RGB565(0x58,0x7c,0x0c), GREEN, YELLOW});

    _wi_load_icon = new BUIicon(LOAD_ICON_POS, icon_load);    
    _wi_load_icon->setPal((const uint16_t[]){BLACK, BLACK, RGB565(0xAC, 0x2A, 0x77)});

    _wi_ma->init(10, 100, MIN_ILOAD, MAX_ILOAD, currentFormat);
    _wi_ma->setFont(&courierFont);
    _wi_ma->setPal((const uint16_t[]){BLACK, YELLOW});
    _wi_voltage->init(100,100, MIN_VLOAD, MAX_VLOAD, voltageFormat);
    _wi_voltage->setPal((const uint16_t[]){BLACK, GREEN});
    _wi_voltage->setFont(&courierFont);    
    _wi_pwr->setPal((const uint16_t[]){BLACK, RGB565(0x54, 0x9c, 0xd6)});
    
    addWidget(_wi_graph);
    addWidget(_wi_ah);
    addWidget(_wi_pwr);
    addWidget(_wi_time);
    addWidget(_wi_voltage);
    addWidget(_wi_ma);
    addWidget(_wi_load_icon);

    _txt_buf = (char*)bui_malloc(10);
    configASSERT(_txt_buf != NULL);
}

void ViewLoad::init(void){
    // Invalidate view
    setInvalid(true);
    // Invalidate widgets
    BUIWidget::invalidateList(getWidgets());
    _wi_graph->redraw();
}

void ViewLoad::draw(void){
    if(isInvalid()){
        DRAW_FillRect(0, 0, LCD_W, LCD_H, BLACK);
        setInvalid(false);
    }
}

void ViewLoad::updateCapacity(int32_t mah){
    xsprintf(_txt_buf, "%umAh", mah);    
    _wi_ah->setText(_txt_buf);
}

void ViewLoad::updateCurrent(uint32_t ma){   
    _wi_ma->setValue(ma);
}

void ViewLoad::updateVoltage(int32_t v){
    _wi_voltage->setValue(v);
}

void ViewLoad::updatePower(int32_t mw){
    if(mw >= 0 && mw < 100000){
        xsprintf(_txt_buf,"%02u.%uW", mw/1000, (mw/100)%10);
    }else{
        xsprintf(_txt_buf, "--.-W");
    }
    _wi_pwr->setText(_txt_buf);
}

void ViewLoad::updateTime(uint32_t ts){
    uint8_t hour = ts / 3600;
    uint8_t min = (ts - (hour*3600)) / 60;

    xsprintf(_txt_buf, "%02u:%02u", hour, min);
    _wi_time->setText(_txt_buf);
}

void ViewLoad::updateGraph(void){
    uint8_t vals[2];
    vals[0] = (_wi_voltage->getValue() * LOAD_GRAPH_HIGHT) / MAX_VLOAD;
    vals[1] = (_wi_ma->getValue() * LOAD_GRAPH_HIGHT) / MAX_ILOAD;
    _wi_graph->addPoint(vals, 2);
}

void ViewLoad::showLoadIcon(uint8_t v){
    _wi_load_icon->setVisible(v);
    _wi_load_icon->setInvalid(true);
}

void ViewLoad::editCC(uint8_t dig){
    _wi_ma->editValue(dig);
}

void ViewLoad::changeCC(uint8_t d){
    _wi_ma->changeValue(d);
}

void ViewLoad::editEndVoltage(uint8_t dig){
    _wi_voltage->editValue(dig);
}

void ViewLoad::changeEndVoltage(uint8_t d){
    _wi_voltage->changeValue(d);
}
