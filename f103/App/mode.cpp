#include <stdio.h>
#include "psu.h"


void Mode::changePlace(int8_t sel){
uint8_t c = place + sel;
    if(c < SET_MAX_DIGITS)
        place = c;
}

void Mode::incrementPlace(int8_t base){
float c = (float)base;
uint8_t d = place;
    while(d--){ c = c / 10; }
    c = *set_value + c;
    if(c >= set_min  && c < set_max){
        *set_value = c;
        setOutput(c, set_max, set_min); // Call function that will configure HW
    }
}

void Mode::printPower(uint16_t x, uint16_t y, float v, float i){
char out[10];
float p = i * v;
    
    if(p < 10){
        sprintf(out, "0%.1fW", p);    
    }else{
        sprintf(out, "%.1fW", p);
    }
    TEXT_print(x,y, out);
}

void Mode::printVoltage(uint16_t x, uint16_t y, float v){
char out[10];
   
    if(v < 10){
        sprintf(out, "0%.1fV", v);
    }else{
        sprintf(out, "%.1fV", v);
    }
    TEXT_print(x,y, out);    
}

