#include "psu.h"

static uint16_t mapPwm(float x, float in_max){
  return (x * (PWM_MAX_VALUE - PWM_MIN_VALUE) / in_max) + PWM_MIN_VALUE;
}

void setOutputVoltage(float val, float max){
    PWM_Set(PWM_OUT_VOLTAGE, mapPwm(val, max));
}

void setOutputCurrent(float val, float max){
    PWM_Set(PWM_OUT_CURRENT, mapPwm(val, max));
}

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
    if(c >=0 && c < set_max){
        *set_value = c;
        setOutput(c, set_max);
    }
}

