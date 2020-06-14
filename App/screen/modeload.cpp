#include "psu.h"
#include "draw.h"

#define LOAD_POWER_POS       0,0
#define LOAD_VOLTAGE_POS     88,17
#define LOAD_TIME_POS        88,25
#define LOAD_CURRENT_POS     88,10

#define LOAD_GRAPH_X         1
#define LOAD_GRAPH_Y         10
#define LOAD_GRAPH_POS       LOAD_GRAPH_X,LOAD_GRAPH_Y
#define LOAD_GRAPH_H         20
#define LOAD_GRAPH_W         70

void plotPoint(uint8_t x, uint8_t y){
   /* if(graph_data[x] > 0){
        LCD_Pixel(x, graph_data[x], BLACK);
    }
    if(y > LOAD_GRAPH_H){
        y = LOAD_GRAPH_H;
    }
    y = LOAD_GRAPH_Y + LOAD_GRAPH_H - y;
    graph_data[x] = y;
    LCD_Pixel(LOAD_GRAPH_X + x, y, WHITE); */
}

void drawHVLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
    for (uint16_t h = y1; h <= y2; h++)
    {
        for (uint16_t w = x1; w <= x2; w++)
        {
            LCD_Pixel(w,h, WHITE);
        }        
    }    
}

void drawGraphAxis(void){
    drawHVLine(LOAD_GRAPH_X, LOAD_GRAPH_Y, LOAD_GRAPH_X, LOAD_GRAPH_Y + LOAD_GRAPH_H);
    drawHVLine(LOAD_GRAPH_X, LOAD_GRAPH_Y + LOAD_GRAPH_H, LOAD_GRAPH_X + LOAD_GRAPH_W, LOAD_GRAPH_Y + LOAD_GRAPH_H);
    memset(graph_data, 0, LOAD_GRAPH_W);
}

void ScreenLoad::redraw(void){
    DRAW_FillRect(0, 0, LCD_W, LCD_H, BLACK);
//TEXT_drawGfx(90,0, (uint8_t*)&icon_load[0]);
    //TEXT_SetFont(&pixelDustFont);
    TEXT_Print(LOAD_POWER_POS,"00.0W");
    TEXT_Print(LOAD_CURRENT_POS,"0.50A");
    TEXT_Print(LOAD_VOLTAGE_POS,"4.1V");
    TEXT_Print(LOAD_TIME_POS,"00:00");
    drawGraphAxis();

    for (uint8_t i = 0; i < LOAD_GRAPH_W; i++)
    {
        plotPoint(i, i/10);
    }    
}

void ScreenLoad::modeSet(){
    if(mode_state == MODEST_NORMAL){
        mode_state = MODEST_SET_V;
    }else {
        mode_state = MODEST_NORMAL;
    }
}

void ScreenLoad::process(psu_t *st){
    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        if(mode_state){
            count = 0;
            switch(BUTTON_VALUE){
                case BUTTON_SET: count = BLINK_TIME_MASK; break;
                case BUTTON_UP:  break;
                case BUTTON_DOWN: break;
                case BUTTON_LEFT:  break;
                case BUTTON_RIGHT:  break;
            }       
        }
    }

    if(mode_state == MODEST_NORMAL){
        //uint16_t *p = &st->adc_v1, i;
        //TEXT_SetFont(&pixelDustFont);        
        //printCurrent(I_POS, st->adc_i1 * CURRENT_PRECISION);
    }else if(mode_state == MODEST_SET_V){
        //TEXT_SetFont(&pixelDustFont);
        if((++count) & BLINK_TIME_MASK){
            TEXT_Print(LOAD_CURRENT_POS,"  ");
        }else{            
            TEXT_Print(LOAD_CURRENT_POS, "0.50A");
        }
    }    
    
}

void ScreenLoad::init(){
    redraw();
}