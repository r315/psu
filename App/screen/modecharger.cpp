#include "psu.h"
#include "draw.h"

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
const float pack_voltages [] = {4.2, 8.4, 12.6, 16.8};
float pack_v_min, pack_v_max;

void changeBtSize(uint8_t *dst, int8_t a){
    uint8_t n = *dst + a;
    if(n > MAX_PACK_SIZE){
        return;
    }
    *dst = n;
    psu_setOutputVoltage(pack_voltages[n]);
}

void ScreenCharger::redraw(void){
    DRAW_FillRect(0, 0, LCD_W, LCD_H, BLACK);
//TEXT_drawGfx(90,0, (uint8_t*)&icon_chr[0]);
    //TEXT_SetFont(&pixelDustFont);
    TEXT_Print(V1_POS,"V1:");
    TEXT_Print(V2_POS,"V2:");
    TEXT_Print(V3_POS,"V3:");
    TEXT_Print(V4_POS,"V4:");
    //TEXT_SetFont(&lcdFont);
    TEXT_Print(BT_SIZE_POS,"1S");
    bt_size = BT1S;    
}

void ScreenCharger::process(){    

    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        if(_screen_state){
            _count = 0;
            switch(BUTTON_VALUE){
                case BUTTON_SET: _count = BLINK_TIME_MASK; break;
                case BUTTON_UP:  changeBtSize(&bt_size, 1); break;
                case BUTTON_DOWN: changeBtSize(&bt_size, -1); break;
                case BUTTON_LEFT:  break;
                case BUTTON_RIGHT:  break;
            }       
        }
    }

    if(_screen_state == SCR_MODE_WORKING){
        // TODO: FIX
        uint16_t *p = NULL, i;
        //TEXT_SetFont(&pixelDustFont);
        for(i = BT1S; i <= bt_size; i++, p+=2){
        //printVoltage(V_POS_OFFSET + V1_POS + i * 8, *p * VOLTAGE_PRECISION);
        }
        for(;i <= MAX_PACK_SIZE; i++ ){
            TEXT_Print(V_POS_OFFSET + V1_POS + i * 8, "-----");   
        }
        //printCurrent(I_POS, st->adc_i1 * CURRENT_PRECISION);
    }else{
        //TEXT_SetFont(&lcdFont);
        if((++_count) & BLINK_TIME_MASK){
            TEXT_Print(BT_SIZE_POS,"  ");
        }else{            
            TEXT_Print(BT_SIZE_POS, bat_type + bt_size * 3);
        }
    }    
}

void ScreenCharger::init(){
    
}