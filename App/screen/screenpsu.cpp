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

static uint16_t vdro_pal[2] = {BLACK, GREEN};
static uint16_t idro_pal[2] = {BLACK, YELLOW};
static uint16_t txt_pal[2] = {BLACK, PINK};
static uint16_t graph_pal[] = {RGB565(5,10,5), RED, GREEN, YELLOW};

void ScreenPsu::init(void){

    graph.init(93, LCD_H - 32, LCD_W - 93, 30, graph_pal);
    redraw();
}

void ScreenPsu::redraw(void){
    DRAW_FillRect(0, 0, LCD_W, LCD_H, BLACK);

    //DRAW_Icon(PSU_ICON_POS, (uint8_t*)&icon_psu[0], BLUE);
    
    DRAW_HLine(0, 15, LCD_W, WHITE);
    DRAW_VLine(92, 0, LCD_H, WHITE);

    printVoltage(set_v, NO_BLANK);
    printCurrent(set_i, NO_BLANK);
    printPower(set_v, set_i);
    graph.redraw();
}

void ScreenPsu::enterModeSet(void){

    switch(_screen_state){
        case MODEST_SET_SHOW:
        case MODEST_NORMAL:
            printVoltage(set_v, NO_BLANK);
            printCurrent(set_i, NO_BLANK);

            _screen_state = MODEST_SET_V;
            set_value = &set_v;
            set_max = MAX_VOLTAGE;
            set_min = MIN_VOLTAGE;
            digit = 1;
            base_place = 10;
            break;

        case MODEST_SET_V:
            _screen_state = MODEST_SET_I;
            set_max = MAX_CURRENT;
            set_min = MIN_CURRENT;
            set_value = &set_i;
            digit = 1;
            base_place = 1;
            break;

        case MODEST_SET_I:
            _screen_state = MODEST_NORMAL;
            psu_setOutputVoltage(set_v);
            psu_setOutputCurrent(set_i);

        default:    
            break;

    }
}

void ScreenPsu::printVoltage(float value, int8_t hide_digit){
    if(value > MAX_VOLTAGE){
        value = MAX_VOLTAGE;
    }

    if(value < 9.9f)
        xsprintf(gOut,"0%.1fV", value);
    else
        xsprintf(gOut,"%.1fV", value);

    if(hide_digit != NO_BLANK){
        if(hide_digit > 1)
            hide_digit++;
        gOut[hide_digit] = ' ';
    }

    TEXT_SetFont(PSU_DRO_FONT);
    TEXT_SetPalette(vdro_pal);
    TEXT_Print(VOLTAGE_DRO_POS, gOut);    
}

void ScreenPsu::printCurrent(float value, int8_t hide_digit){

    if(value > MAX_CURRENT){
        value = MAX_CURRENT;
    }

    xsprintf(gOut,"%.2fA", value);

    if(hide_digit != NO_BLANK){
        if(hide_digit > 0)
            hide_digit++;
        gOut[hide_digit] = ' ';
    }

    TEXT_SetFont(PSU_DRO_FONT);
    TEXT_SetPalette(idro_pal);
    TEXT_Print(CURRENT_DRO_POS, gOut);
}

void ScreenPsu::printPower(float v, float i){
float p = i * v;
    
    if(p > MAX_CURRENT * MAX_VOLTAGE){
        p = MAX_CURRENT * MAX_VOLTAGE;
    }

    xsprintf(gOut, "%.1fW ", p);    
    
    TEXT_SetFont(PSU_TEXT_FONT);
    TEXT_SetPalette(txt_pal);
    TEXT_Print(POWER_DRO_POS, gOut);
}

void ScreenPsu::process(){
float i, v;

    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        if(_screen_state != MODEST_NORMAL){
            count = 0;
            switch(BUTTON_VALUE){
                case BUTTON_SET: count = BLINK_TIME_MASK; break;
                case BUTTON_UP: changeDigit(base_place); break;
                case BUTTON_DOWN: changeDigit(-base_place); break;
                case BUTTON_LEFT: selectDigit(-1); break;
                case BUTTON_RIGHT: selectDigit(1); break;
            }       
        }
    }

    switch(_screen_state){
        case MODEST_NORMAL:
            if(IS_OE_FLAG_SET(st->flags) == FALSE){
                mode_state = MODEST_SET_SHOW;
                break;
            }

            if(IS_AD_FLAG_SET(st->flags)){        
                v = psu_getVoltage();
                i = psu_getCurrent();
                printPower(v, i);        
                printVoltage(v, NO_BLANK);
                printCurrent(i, NO_BLANK);
                graph.addPoint((int)v, 0);
                graph.addPoint((int)i, 1);
                graph.nextPoint();
                graph.update();                
            }
            break;
        
        case MODEST_SET_V:
            if((++count) & BLINK_TIME_MASK){
                printVoltage(set_v, digit);
            }else{
                printVoltage(set_v, NO_BLANK);
            }
            break;

        case MODEST_SET_I:
            if((++count) & BLINK_TIME_MASK){
                printCurrent(set_i, digit);
            }else{
                printCurrent(set_i, NO_BLANK);
            }
            break;

        case MODEST_SET_SHOW:
            if(psu_getOutputEnable()){
                _screen_state = MODEST_NORMAL;
                break;
            }
            printVoltage(set_v, NO_BLANK);
            printCurrent(set_i, NO_BLANK);
            break;

        default:
            break;
    }
}