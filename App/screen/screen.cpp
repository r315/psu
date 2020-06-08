#include <stdio.h>
#include "psu.h"


void Screen::selectDigit(int8_t sel){
uint8_t c = digit + sel;
    if(c < SET_MAX_DIGITS)
        digit = c;
}

void Screen::changeDigit(int8_t base){
float c = (float)base;
uint8_t d = digit;
    while(d--){ c = c / 10.0f; }
    c = *set_value + c;
    if(c >= set_min  && c < set_max){
        *set_value = c;
    }
}
