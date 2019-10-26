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

