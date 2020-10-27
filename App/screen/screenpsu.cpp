#include <math.h>
#include <stdio.h>
#include "psu.h"
#include "draw.h"
#include "text.h"
#include "graph.h"
#include "font.h"

#define VOLTAGE_DRO_POS_X           0
#define VOLTAGE_DRO_POS_Y           16
#define VOLTAGE_DRO_POS             VOLTAGE_DRO_POS_X,VOLTAGE_DRO_POS_Y
#define CURRENT_DRO_POS_X           0
#define CURRENT_DRO_POS_Y           16+32
#define CURRENT_DRO_POS             CURRENT_DRO_POS_X,CURRENT_DRO_POS_Y
#define POWER_DRO_POS               0,0

#define PSU_ICON_POS                97,0

#define PSU_TEXT_FONT               &courierFont
#define PSU_DRO_FONT                &GroteskBold16x32

#define PSU_LINE_COLOR              RGB565(5,10,5)

static const uint16_t vdro_pal[2] = {BLACK, GREEN};
static const uint16_t idro_pal[2] = {BLACK, YELLOW};
static const uint16_t pwr_pal[2] = {BLACK, SKYBLUE};
static const uint16_t txt_pal[2] = {BLACK, WHITE};
static const uint16_t graph_pal[] = {RGB565(5,10,10), RED, GREEN, YELLOW};

void ScreenPsu::init(void){
    preset_t *preset = app_getPreset();
    _set_v = preset->v;
    _set_i = preset->i;
    _graph.init(93, LCD_H - 32, LCD_W - 93, 30, graph_pal);
    _screen_state = SCR_MODE_IDLE;
    redraw();
}

void ScreenPsu::redraw(void){
    DRAW_FillRect(0, 0, LCD_W, LCD_H, BLACK);

    //DRAW_Icon(PSU_ICON_POS, (uint8_t*)&icon_psu[0], BLUE);
    
    DRAW_HLine(0, 15, LCD_W, PSU_LINE_COLOR);
    DRAW_VLine(92, 0, LCD_H, PSU_LINE_COLOR);

    updateVoltage(BLINK_OFF, _set_v);
    updateCurrent(BLINK_OFF, _set_i);
    //printPower(set_v, set_i);
    _graph.redraw();

    updatePresetIndex();
}

void ScreenPsu::updatePresetIndex(){
    uint8_t index = app_getPreset() - app_getPresetList();
    xsprintf(gOut,"M%u", index + 1);

    TEXT_SetFont(PSU_TEXT_FONT);
    TEXT_SetPalette(txt_pal);
    TEXT_Print(LCD_W - 64, 0, gOut);
}

void ScreenPsu::updateVoltage(uint8_t hide_digit, uint32_t mv){
    printVoltage(VOLTAGE_DRO_POS, PSU_DRO_FONT, vdro_pal, hide_digit, mv);
}

void ScreenPsu::updateCurrent(uint8_t hide_digit, uint32_t ma){
    printCurrent(CURRENT_DRO_POS, PSU_DRO_FONT, idro_pal, hide_digit, ma);
}

void ScreenPsu::updatePower(uint32_t pwr){
    printPower(POWER_DRO_POS, PSU_TEXT_FONT, pwr_pal, BLINK_OFF, pwr);
}

void ScreenPsu::process(){
uint32_t i, v;
preset_t *pre;
static uint32_t *ptr_set;

// TODO: Improve state machine
    if(BUTTON_GetEvents() == BUTTON_PRESSED){        
        if(BUTTON_VALUE == BUTTON_SET){
            switch(_screen_state){
                case SCR_MODE_IDLE:
                case SCR_MODE_NORMAL:
                    _screen_state = SCR_MODE_SET_V;
                    configSetting(1000, 100, 10000, MIN_VOLTAGE, MAX_VOLTAGE);
                    ptr_set = &_set_v;
                    break;

                case SCR_MODE_SET_V:
                    _screen_state = SCR_MODE_SET_I;
                    updateVoltage(BLINK_OFF, _set_v);
                    configSetting(100, 10, 1000, MIN_CURRENT, MAX_CURRENT);
                    ptr_set = &_set_i;
                    break;

                case SCR_MODE_SET_I:
                    // Exit mode set state
                    _screen_state = SCR_MODE_NORMAL;
                    updateCurrent(BLINK_OFF, _set_i);
                    pre = app_getPreset();
                    pre->v = _set_v;
                    pre->i = _set_i;
                    psu_setOutputVoltage(_set_v);
                    psu_setOutputCurrent(_set_i);
                    break;

                default:    
                    break;

            }
        }
        
        if(_screen_state == SCR_MODE_SET_V ||_screen_state == SCR_MODE_SET_I){
            _count = 0;
            switch(BUTTON_VALUE){
                case BUTTON_SET: _count = BLINK_TIME_MASK; break;
                case BUTTON_UP: addPow(ptr_set); break;
                case BUTTON_DOWN: subPow(ptr_set); break;
                case BUTTON_LEFT: mulPow(); break;
                case BUTTON_RIGHT: divPow(); break;
                case BUTTON_MEM: 
                    // Cancel set
                    _screen_state = SCR_MODE_NORMAL; 
                    preset_t *pre = app_getPresetList();
                    _set_v = pre->v;
                    _set_i = pre->i;
                    break;
            }
        }
    }

    switch(_screen_state){
        case SCR_MODE_NORMAL:
            if(!psu_getOutputEnable()){
                _screen_state = SCR_MODE_IDLE;
                preset_t *pre = app_getPresetList();
                updateVoltage(BLINK_OFF,pre->v);
                updateCurrent(BLINK_OFF, pre->i);
                break;
            }

            if(psu_AdcReady()){
                v = psu_getVoltage();
                i = psu_getCurrent();
                updatePower(v * i);        
                updateVoltage(BLINK_OFF, v);
                updateCurrent(BLINK_OFF, i);
                _graph.addPoint((int)v, 0);
                _graph.addPoint((int)i, 1);
                _graph.nextPoint();
                _graph.update();                
            }
            break;
        
        case SCR_MODE_SET_V:
            updateVoltage(BLINK_ON, _set_v);
            break;

        case SCR_MODE_SET_I:
            updateCurrent(BLINK_ON, _set_i);            
            break;

        case SCR_MODE_IDLE:
            if(psu_getOutputEnable()){
                _screen_state = SCR_MODE_NORMAL;
            }            
            break;

        default:
            break;
    }
}