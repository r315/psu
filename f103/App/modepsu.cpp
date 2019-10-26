#include "psu.h"
#include <math.h>
#include <stdio.h>

#define VOLTAGE_PLACES              1
#define CURRENT_PLACES              2


float mapAdc(uint16_t adcvalue, float reference, float max){

return (adcvalue * (reference / max));
}

void ModePsu::redraw(void){
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
    //TEXT_setFont(&defaultBoldFont);
    //TEXT_setFont(&lcdFont);
    TEXT_setFont(&pixelDustFont);
    //TEXT_setFont(&defaultFont);
    TEXT_print(0,0, "00.0W");
    TEXT_setFont(&font_seven_seg);
    TEXT_dro(VOLTAGE_DRO_POS, 0, VOLTAGE_PLACES, NO_BLANK);
    TEXT_dro(CURRENT_DRO_POS, 0, CURRENT_PLACES, NO_BLANK);
    TEXT_drawGfx(PSU_ICON_POS, (uint8_t*)&icon_psu[0]);
}

void ModePsu::modeSet(void){
    if(mode_set == SET_OFF){
        mode_set = SET_VOLTAGE;
        set_value = &set_v;
        set_max = MAX_VOLTAGE;
        set_min = MIN_VOLTAGE;
        place = 1;
        base_place = 10;
        setOutput = setOutputVoltage;
    }else if(mode_set == SET_VOLTAGE){
        mode_set = SET_CURRENT; 
        set_max = MAX_CURRENT;
        set_min = MIN_CURRENT;
        set_value = &set_i;
        place = 1;
        base_place = 1;
        setOutput = setOutputCurrent;
    }else{
        mode_set = SET_OFF;
    }
}

void ModePsu::process(State *st){
float i, v, p;
char out[10];

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
        TEXT_setFont(&pixelDustFont);
        v = st->adc_out_v * VOLTAGE_PERCISION;
        i = st->adc_out_i * CURRENT_PERCISION;
        p = i * v;
        
        if(p < 10){
            sprintf(out, "0%.1fW", p);    
        }else{
            sprintf(out, "%.1fW", p);
        }
        TEXT_print(0,0, out);
        TEXT_setFont(&font_seven_seg);
        TEXT_dro(VOLTAGE_DRO_POS, v, VOLTAGE_PLACES, NO_BLANK);
        TEXT_dro(CURRENT_DRO_POS, i, CURRENT_PLACES, NO_BLANK);
    }    
    else if((++count) & BLINK_TIME_MASK){
        if(mode_set == SET_VOLTAGE){
            TEXT_dro(VOLTAGE_DRO_POS, set_v, VOLTAGE_PLACES, place);
            TEXT_dro(CURRENT_DRO_POS, set_i, CURRENT_PLACES, NO_BLANK);
        }else{
            TEXT_dro(VOLTAGE_DRO_POS, set_v, VOLTAGE_PLACES, NO_BLANK);
            TEXT_dro(CURRENT_DRO_POS, set_i, CURRENT_PLACES, place);
        }            
    }else{
        TEXT_dro(VOLTAGE_DRO_POS, set_v, VOLTAGE_PLACES, NO_BLANK);
        TEXT_dro(CURRENT_DRO_POS, set_i, CURRENT_PLACES, NO_BLANK);
    }
    
}