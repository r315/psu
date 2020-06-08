#include <math.h>
#include <stdio.h>
#include "psu.h"
#include "draw.h"
#include "text.h"
#include "adcmux.h"

#define VOLTAGE_DRO_POS_X           0
#define VOLTAGE_DRO_POS_Y           16
#define VOLTAGE_DRO_POS             VOLTAGE_DRO_POS_X,VOLTAGE_DRO_POS_Y
#define CURRENT_DRO_POS_X           0
#define CURRENT_DRO_POS_Y           16+32
#define CURRENT_DRO_POS             CURRENT_DRO_POS_X,CURRENT_DRO_POS_Y
#define POWER_DRO_POS               0,0

#define PSU_ICON_POS                97,0

#define PSU_TEXT_FONT               &courierFont
#define PSU_DRO_FONT                &GroteskBold16x32 //&font_seven_seg

static char gout[7];
static uint16_t vdro_pal[2] = {BLACK, GREEN};
static uint16_t idro_pal[2] = {BLACK, YELLOW};
static uint16_t txt_pal[2] = {BLACK, PINK};
static const uint8_t adc_seq[] = {0 , 1};

volatile uint16_t v1,v2, update;

extern "C" void psu_cb(uint16_t *data){
    v1 = data[0];
    v2 = data[1];
    update = true;
}

void ScreenPsu::initPreSetValues(float v_set, float i_set){
    set_v = v_set;
    set_i = i_set;
}

void ScreenPsu::init(void){

    update = false;

    ADCMUX_StartSequence((uint8_t*)adc_seq, sizeof(adc_seq), psu_cb);

    redraw();
}

void ScreenPsu::redraw(void){
    DRAW_FillRect(0, 0, LCD_W, LCD_H, BLACK);

    DRAW_Icon(PSU_ICON_POS, (uint8_t*)&icon_psu[0], BLUE);
    
    DRAW_HLine(0, 15, LCD_W, WHITE);
    DRAW_VLine(92, 0, LCD_H, WHITE);

    printVoltage(set_v, NO_BLANK);
    printCurrent(set_i, NO_BLANK);
    printPower(set_v, set_i);
}

void ScreenPsu::modeSet(void){

    switch(mode_state){
        case MODEST_SET_SHOW:
        case MODEST_NORMAL:
            printVoltage(set_v, NO_BLANK);
            printCurrent(set_i, NO_BLANK);

            mode_state = MODEST_SET_V;
            set_value = &set_v;
            set_max = MAX_VOLTAGE;
            set_min = MIN_VOLTAGE;
            digit = 1;
            base_place = 10;
            break;

        case MODEST_SET_V:
            mode_state = MODEST_SET_I;
            set_max = MAX_CURRENT;
            set_min = MIN_CURRENT;
            set_value = &set_i;
            digit = 1;
            base_place = 1;
            break;

        case MODEST_SET_I:
            mode_state = MODEST_NORMAL;
            psu_setOutputVoltage(set_v, MAX_VOLTAGE, MIN_VOLTAGE);
            psu_setOutputCurrent(set_i, MAX_CURRENT, MIN_CURRENT);

        default:    
            break;

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
    
    sprintf(gout, "%.1fW ", p);    

    TEXT_SetFont(PSU_TEXT_FONT);
    TEXT_SetPalette(txt_pal);
    TEXT_Print(POWER_DRO_POS, gout);
}

void ScreenPsu::process(State *st){
float i, v;

    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        if(mode_state != MODEST_NORMAL){
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

    switch(mode_state){
        case MODEST_NORMAL:
            if(st->output_en == FALSE){
                mode_state = MODEST_SET_SHOW;
                break;
            }

            if(update){        
                v = v1 * VOLTAGE_PRECISION;
                i = v2 * CURRENT_PRECISION;        
                printPower(v, i);        
                printVoltage(v, NO_BLANK);
                printCurrent(i, NO_BLANK);
                update = false;
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
            if(st->output_en == TRUE){
                mode_state = MODEST_NORMAL;
                break;
            }
            printVoltage(set_v, NO_BLANK);
            printCurrent(set_i, NO_BLANK);
            break;

        default:
            break;
    }
}