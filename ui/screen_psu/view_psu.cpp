#include "bui.h"
#include "view_psu.h"
#include "wi_dro.h"
#include "psu.h"

#define VOLTAGE_POS_X           0
#define VOLTAGE_POS_Y           16
#define CURRENT_POS_X           0
#define CURRENT_POS_Y           16+32
#define POWER_DRO_POS           0,0

#define PSU_GRAPH_HIGHT         30
#define PSU_GRAPH_WIDTH         60
#define MAX_TEXT_LEN            10

#define PSU_OUTPUT_ICON_POS             160-16,3

static const uint8_t psu_icon_out[] = {15,8,
    0x7f,0xfc,0xc5,0x46,0xd5,0x6e,0xd5,0x6e,0xd5,0x6e,0xd5,0x6e,0xc4,0x6e,0x7f,0xfc
};

ViewPsu::ViewPsu(): 
    _wi_power(POWER_DRO_POS),
    _wi_current(CURRENT_POS_X, CURRENT_POS_Y),
    _wi_voltage(VOLTAGE_POS_X, VOLTAGE_POS_Y),
    _wi_graph(LCD_W - PSU_GRAPH_WIDTH, LCD_H - PSU_GRAPH_HIGHT - 2, 
             PSU_GRAPH_WIDTH, PSU_GRAPH_HIGHT, 
             2, 
             (const uint16_t[]){0x18E3, RED, GREEN, YELLOW}),
    _wi_out_icon(PSU_OUTPUT_ICON_POS, psu_icon_out)
{

    _wi_voltage.init(100, 100, MIN_VOLTAGE, MAX_VOLTAGE, voltageFormat);
    _wi_current.init(10, 100, MIN_CURRENT, MAX_CURRENT, currentFormat);

    _wi_out_icon.setPal((const uint16_t[]){BLACK, BLACK, RED});
    
    addWidget(&_wi_power);
    addWidget(&_wi_voltage);
    addWidget(&_wi_current);
    addWidget(&_wi_graph);
    addWidget(&_wi_out_icon);
}

void ViewPsu::init(void){
    // Invalidate view
    setInvalid(true);
    // Invalidate widgets
    BUIWidget::invalidateList(getWidgets());
    _wi_graph.redraw();
}

void ViewPsu::draw(){
    if(isInvalid()){
        DRAW_FillRect(0, 0, LCD_W, LCD_H, BLACK);
        setInvalid(false);
    }
}

void ViewPsu::updateVoltage(uint32_t mv){    
    _wi_voltage.setValue(mv);
}

void ViewPsu::updateCurrent(uint32_t ma){
    _wi_current.setValue(ma);
}

void ViewPsu::updatePower(int32_t mw){
    char out[5];
    if(mw >= 0 && mw < 100000){
        xsprintf(out,"%02u.%uW", mw/1000, (mw/100)%10);
    }else{
        xsprintf(out,"--.-W");
    }
    _wi_power.setText(out);
}

void ViewPsu::updateGraph(void){
    uint8_t vals[2];
    vals[0] = _wi_voltage.getValue() * PSU_GRAPH_HIGHT / MAX_VOLTAGE;
    vals[1] = _wi_current.getValue() * PSU_GRAPH_HIGHT / MAX_CURRENT;
    _wi_graph.addPoint(vals, 2);
}

void ViewPsu::editVoltage(uint8_t dig){
    _wi_voltage.editValue(dig);
}

void ViewPsu::editCurrent(uint8_t dig){
    _wi_current.editValue(dig);
}

void ViewPsu::changeVoltage(uint8_t d){
    _wi_voltage.changeValue(d);
}

void ViewPsu::changeCurrent(uint8_t d){
    _wi_current.changeValue(d);
}

void ViewPsu::showOutIcon(uint8_t v){
    _wi_out_icon.setVisible(v);
    _wi_out_icon.setInvalid(true);
}

