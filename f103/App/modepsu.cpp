#include "psu.h"
#include <math.h>


static uint16_t mapPwm(float x, float in_max){
  return (x * (PWM_MAX_VALUE - PWM_MIN_VALUE) / in_max) + PWM_MIN_VALUE;
}

static void setOutputVoltage(float val, float max){
    PWM_Set(PWM_OUT_VOLTAGE, mapPwm(val, max));
}

static void setOutputCurrent(float val, float max){
    PWM_Set(PWM_OUT_CURRENT, mapPwm(val, max));
}

static void changePlace(uint8_t *cur, int8_t sel){
uint8_t c = *cur + sel;
    if(c < SET_MAX_DIGITS)
        *cur = c;
}


void ModePsu::incrementPlace(int8_t base){
float c = (float)base;
uint8_t d = place;
    while(d--){ c = c / 10; }
    c = *set_value + c;
    if(c >=0 && c < set_max){
        *set_value = c;
        setOutput(c, set_max);
    }
}

void ModePsu::redraw(void){
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
    //TEXT_setFont(&defaultBoldFont);
    //TEXT_setFont(&lcdFont);
    TEXT_setFont(&pixelDustFont);
    //TEXT_setFont(&defaultFont);
    TEXT_print(0,0, "88.8W");
    TEXT_setFont(&font_seven_seg);
    TEXT_dro(VOLTAGE_DRO_POS, 0, VOLTAGE_PRECISION, NO_BLANK);
    TEXT_dro(CURRENT_DRO_POS, 0, CURRENT_PRECISION, NO_BLANK);
    TEXT_drawGfx(PSU_ICON_POS, (uint8_t*)&icon_psu[0]);
}

void ModePsu::modeSet(void){
    if(mode_set == SET_OFF){
        mode_set = SET_VOLTAGE;
        set_value = &set_v;
        set_max = SET_MAX_VOLTAGE;
        place = 1;
        base_place = 10;
        setOutput = setOutputVoltage;
    }else if(mode_set == SET_VOLTAGE){
        mode_set = SET_CURRENT; 
        set_max = SET_MAX_CURRENT;
        set_value = &set_a;
        place = 1;
        base_place = 1;
        setOutput = setOutputCurrent;
    }else{
        mode_set = SET_OFF;
    }
}

void ModePsu::process(State *st){
    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        if(mode_set){
            count = 0;
            switch(BUTTON_VALUE){
                case BUTTON_SET: count = BLINK_TIME_MASK; break;
                case BUTTON_UP: incrementPlace(base_place); break;
                case BUTTON_DOWN: incrementPlace(-base_place); break;
                case BUTTON_LEFT: changePlace(&place, -1); break;
                case BUTTON_RIGHT: changePlace(&place, 1); break;
            }       
        }
    }    

        if(mode_set != SET_OFF && (++count) & BLINK_TIME_MASK){
             if(mode_set == SET_VOLTAGE){
                TEXT_dro(VOLTAGE_DRO_POS, set_v, VOLTAGE_PRECISION, place);
                TEXT_dro(CURRENT_DRO_POS, set_a, CURRENT_PRECISION, NO_BLANK);
            }else{
                TEXT_dro(VOLTAGE_DRO_POS, set_v, VOLTAGE_PRECISION, NO_BLANK);
                TEXT_dro(CURRENT_DRO_POS, set_a, CURRENT_PRECISION, place);
            }            
        }else{
            TEXT_dro(VOLTAGE_DRO_POS, set_v, VOLTAGE_PRECISION, NO_BLANK);
            TEXT_dro(CURRENT_DRO_POS, set_a, CURRENT_PRECISION, NO_BLANK);
        }
    
}