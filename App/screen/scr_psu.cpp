#include "bui.h"
#include "scr_psu.h"

#define VOLTAGE_POS_X           0
#define VOLTAGE_POS_Y           16
#define CURRENT_POS_X           0
#define CURRENT_POS_Y           16+32
#define POWER_DRO_POS           0,0

#define PSU_GRAPH_HIGHT         30
#define PSU_GRAPH_WIDTH         93
#define MAX_TEXT_LEN            10

SCRpsu::SCRpsu(): 
    wi_power(POWER_DRO_POS),
    wi_current(CURRENT_POS_X, CURRENT_POS_Y),
    wi_voltage(VOLTAGE_POS_X, VOLTAGE_POS_Y),
    wi_graph(PSU_GRAPH_WIDTH, LCD_H - PSU_GRAPH_HIGHT - 2, 
             LCD_W - PSU_GRAPH_WIDTH, PSU_GRAPH_HIGHT, 
             2, 
             (const uint16_t[]){0x18E3, RED, GREEN, YELLOW})
{
    
    wi_current.setFont(&GroteskBold16x32);
    wi_voltage.setFont(&GroteskBold16x32);

    wi_voltage.setPal((const uint16_t []){0x18E3, 0xEF58});
    wi_current.setPal((const uint16_t []){0x18E3, 0xEF58});
    //wi_power.setPal((const uint16_t []){0x18E3, 0xEF58});
    
    addWidget(&wi_power);
    addWidget(&wi_voltage);
    addWidget(&wi_current);
    addWidget(&wi_graph);

    _txt_buffer = (char*)bui_malloc(MAX_TEXT_LEN);

    //wi_power.setText("0.0W");
    //wi_power.setPal((const uint16_t []){PINK,BLUE});
    
    wi_current.setText("0.0A");
    wi_voltage.setText("0.0V");
}

SCRpsu::~SCRpsu(){
    if(_txt_buffer != NULL){
        bui_free(_txt_buffer);
    }
}

void SCRpsu::updateVoltage(uint32_t mv){
    //xsprintf(gOut,"%02u.%uV", mv/1000, (mv/100)%10);
    //_printValue(x, y, font, pal, blink_digit, gOut);
}

void SCRpsu::updateCurrent(uint32_t mva){
    //xsprintf(gOut,"%d.%02uA", ma/1000, (ma/10)%100);
    //_printValue(x, y, font, pal, blink_digit, gOut);    
}

void SCRpsu::updatePower(uint32_t mw){
    //xsprintf(gOut,"%02u.%uW", mw/1000, (mw/100)%10);
    //_printValue(x, y, font, pal, blink_digit, gOut);
}

void SCRpsu::keyPressed(uint32_t key){
    switch(key){
        case BUTTON_SET:
            //nextScreen();
            break;

        default:
            xsprintf(_txt_buffer,"%02X  ", key);
            wi_power.setText(_txt_buffer);
            break;
    }
}

void SCRpsu::keyReleased(uint32_t key){

}