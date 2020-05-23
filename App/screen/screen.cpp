#include <stdio.h>
#include "psu.h"


void Screen::changePlace(int8_t sel){
uint8_t c = place + sel;
    if(c < SET_MAX_DIGITS)
        place = c;
}

void Screen::incrementPlace(int8_t base){
float c = (float)base;
uint8_t d = place;
    while(d--){ c = c / 10.0f; }
    c = *set_value + c;
    if(c >= set_min  && c < set_max){
        *set_value = c;
        setOutput(c, set_max, set_min); // Call function that will configure HW
    }
}
