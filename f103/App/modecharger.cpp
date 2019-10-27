#include "psu.h"

#define V_POS_OFFSET    22
#define V1_POS          0,0
#define V2_POS          0,8
#define V3_POS          0,16
#define V4_POS          0,24
#define I_POS           85,24
#define BT_SIZE_POS     100,12
#define MAX_PACK_SIZE   BT4S

enum BAT_TYPE {BT1S = 0, BT2S, BT3S, BT4S};
const char bat_type[] = {"1S 2S 3S 4S"};

void changeBtSize(uint8_t *dst, int8_t a){
    uint8_t n = *dst + a;
    if(n > MAX_PACK_SIZE){
        return;
    }
    *dst = n;
}

void ModeCharger::redraw(void){
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
    TEXT_drawGfx(90,0, (uint8_t*)&icon_chr[0]);
    TEXT_setFont(&pixelDustFont);
    TEXT_print(V1_POS,"V1:");
    TEXT_print(V2_POS,"V2:");
    TEXT_print(V3_POS,"V3:");
    TEXT_print(V4_POS,"V4:");
    TEXT_setFont(&lcdFont);
    TEXT_print(BT_SIZE_POS,"1S");
    bt_size = BT1S;    
}

void ModeCharger::modeSet(){
    if(mode_set == SET_OFF){
        mode_set = SET_M1;
    }else {
        mode_set = SET_OFF;
    }  
}

void ModeCharger::process(State *st){
    

    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        if(mode_set){
            count = 0;
            switch(BUTTON_VALUE){
                case BUTTON_SET: count = BLINK_TIME_MASK; break;
                case BUTTON_UP:  changeBtSize(&bt_size, 1); break;
                case BUTTON_DOWN: changeBtSize(&bt_size, -1); break;
                case BUTTON_LEFT:  break;
                case BUTTON_RIGHT:  break;
            }       
        }
    }

    if(mode_set == SET_OFF){
        uint16_t *p = &st->adc_v1, i;
        TEXT_setFont(&pixelDustFont);
        for(i = BT1S; i <= bt_size; i++, p+=2){
        printVoltage(V_POS_OFFSET + V1_POS + i * 8, *p * VOLTAGE_PERCISION);
        }
        for(;i <= MAX_PACK_SIZE; i++ ){
            TEXT_print(V_POS_OFFSET + V1_POS + i * 8, "-----");   
        }
        printCurrent(I_POS, st->adc_i1 * CURRENT_PERCISION);
    }else{
        TEXT_setFont(&lcdFont);
        if((++count) & BLINK_TIME_MASK){
            TEXT_print(BT_SIZE_POS,"  ");
        }else{            
            TEXT_print(BT_SIZE_POS, bat_type + bt_size * 3);
        }
    }
    
    
}