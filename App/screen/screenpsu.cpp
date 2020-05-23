#include <math.h>
#include <stdio.h>
#include "psu.h"
#include "draw.h"
#include "text.h"

#define VOLTAGE_DRO_POS_X           0
#define VOLTAGE_DRO_POS_Y           16
#define VOLTAGE_DRO_POS             VOLTAGE_DRO_POS_X,VOLTAGE_DRO_POS_Y
#define CURRENT_DRO_POS_X           0
#define CURRENT_DRO_POS_Y           16+32
#define CURRENT_DRO_POS             CURRENT_DRO_POS_X,CURRENT_DRO_POS_Y
#define POWER_DRO_POS               0,0

#define PSU_ICON_POS                94,0

#define PSU_TEXT_FONT               &courierFont
#define PSU_DRO_FONT                &GroteskBold16x32 //&font_seven_seg

static char gout[7];
static uint16_t vdro_pal[2] = {BLACK, GREEN};
static uint16_t idro_pal[2] = {BLACK, YELLOW};
static uint16_t txt_pal[2] = {BLACK, WHITE};

void ScreenPsu::startValues(float v_last, float i_last) {
    set_v = v_last;
    set_i = i_last;
}

void ScreenPsu::redraw(void){
    LCD_FillRect(0, 0, LCD_W, LCD_H, BLACK);
    
    DRAW_Icon(PSU_ICON_POS, (uint8_t*)&icon_psu[0], BLUE);
    
    LCD_FillRect(0, 15, LCD_W, 1, WHITE);
    LCD_FillRect(92, 0, 1, LCD_H, WHITE);

    printVoltage(0.0f, NO_BLANK);
    printCurrent(0.0f, NO_BLANK);
    printPower(0.0f, 0.0f);
}

void ScreenPsu::modeSet(void){
    if(mode_set == SET_OFF){
        mode_set = SET_M1;
        set_value = &set_v;
        set_max = MAX_VOLTAGE;
        set_min = MIN_VOLTAGE;
        place = 1;
        base_place = 10;
        setOutput = app_setOutputVoltage;
    }else if(mode_set == SET_M1){
        mode_set = SET_M2; 
        set_max = MAX_CURRENT;
        set_min = MIN_CURRENT;
        set_value = &set_i;
        place = 1;
        base_place = 1;
        setOutput = app_setOutputCurrent;
    }else{
        mode_set = SET_OFF;
    }
}

void ScreenPsu::printVoltage(float value, int8_t hide_digit){
    if(value < 9.9f)
        sprintf(gout,"0%.1fV", value);
    else
        sprintf(gout,"%.1fV", value);

    if(hide_digit != NO_BLANK){
        if(hide_digit > 1)
            hide_digit++;
        gout[hide_digit] = ' ';
    }

    TEXT_SetFont(PSU_DRO_FONT);
    TEXT_SetPalette(vdro_pal);
    TEXT_Print(VOLTAGE_DRO_POS, gout);    
}

void ScreenPsu::printCurrent(float value, int8_t hide_digit){
    sprintf(gout,"%.2fA", value);

    if(hide_digit != NO_BLANK){
        if(hide_digit > 0)
            hide_digit++;
        gout[hide_digit] = ' ';
    }

    TEXT_SetFont(PSU_DRO_FONT);
    TEXT_SetPalette(idro_pal);
    TEXT_Print(CURRENT_DRO_POS, gout);
}

void ScreenPsu::printPower(float v, float i){
float p = i * v;
    
    if(p < 10.0f){
        sprintf(gout, "0%.1fW", p);    
    }else{
        sprintf(gout, "%.1fW", p);
    }

    TEXT_SetFont(PSU_TEXT_FONT);
    TEXT_SetPalette(txt_pal);
    TEXT_Print(POWER_DRO_POS, gout);
}

void ScreenPsu::process(State *st){
float i, v;

    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        if(mode_set){
            count = 0;
            switch(BUTTON_VALUE){
                case BUTTON_SET: count = BLINK_TIME_MASK; break;
                case BUTTON_UP: incrementPlace(base_place); break;
                case BUTTON_DOWN: incrementPlace(-base_place); break;
                case BUTTON_LEFT: changePlace(-1); break;
                case BUTTON_RIGHT: changePlace(1); break;
            }       
        }
    }

    if(st->flags & STATE_FLAG_DISPLAY)
        return;

    if(mode_set == SET_OFF){        
        v = st->adc_v1 * VOLTAGE_PRECISION;
        i = st->adc_i1 * CURRENT_PRECISION;        
        printPower(v, i);        
        printVoltage(v, NO_BLANK);
        printCurrent(i, NO_BLANK);
    }    
    else if((++count) & BLINK_TIME_MASK){
        if(mode_set == SET_M1){
            printVoltage(set_v, place);
            printCurrent(set_i, NO_BLANK);
        }else{
            printVoltage(set_v, NO_BLANK);
            printCurrent(set_i, place);
        }            
    }else{
        printVoltage(set_v, NO_BLANK);
        printCurrent(set_i, NO_BLANK);
    }    
}