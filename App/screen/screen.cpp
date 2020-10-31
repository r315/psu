#include <stdio.h>
#include <strfunc.h>
#include "psu.h"


void Screen::divPow(void){
    if(_pow > _min_pow) 
        _pow /= 10;
}

void Screen::mulPow(void){
    if(_pow < _max_pow)
        _pow *= 10;
}

void Screen::addPow(uint32_t *val){
    _addPow(val, _pow);
}

void Screen::subPow(uint32_t *val){
    _addPow(val, -_pow);
}

/**
 * @brief add two values if result is in range defined by _min_set and _max_set.
 * 
 * \param value : IN/OUT pointer to value
 * \param pow : ten's power to be added to value
 * */
void Screen::_addPow(uint32_t *value, int32_t pow){
int32_t tmp;    
    tmp = *value + pow; 
    if(tmp <= _max_set && tmp >= _min_set){
        *value = tmp;
    }
}

void Screen::_printValue(uint16_t x, uint16_t y, font_t *font, const uint16_t *pal, uint8_t blink_digit, const char *str){
        
    if(blink_digit && (++_count) & BLINK_TIME_MASK){
        uint32_t cur_dig = _min_pow;  // Right most digit
        blink_digit = 3;              // is located on index 3 of the string

        while(cur_dig < _pow){
            blink_digit--;
            if(gOut[blink_digit] == '.'){
                blink_digit--;
            }
            cur_dig *= 10;
        }
        gOut[blink_digit] = ' ';
    }

    TEXT_SetPalette(pal);
    TEXT_SetFont(font);
    TEXT_Print(x,y, str);
}

void Screen::printCurrent(uint16_t x, uint16_t y, font_t *font, const uint16_t *pal, uint8_t blink_digit, uint32_t ma){
    xsprintf(gOut,"%d.%02uA", ma/1000, (ma/10)%100);
    _printValue(x, y, font, pal, blink_digit, gOut);
}

void Screen::printVoltage(uint16_t x, uint16_t y, font_t *font, const uint16_t *pal, uint8_t blink_digit, uint32_t mv){
    xsprintf(gOut,"%02u.%uV", mv/1000, (mv/100)%10);
    _printValue(x, y, font, pal, blink_digit, gOut);
}

void Screen::printPower(uint16_t x, uint16_t y, font_t *font, const uint16_t *pal, uint8_t blink_digit, uint32_t mw){
    xsprintf(gOut,"%02u.%uW", mw/1000, (mw/100)%10);
    _printValue(x, y, font, pal, blink_digit, gOut);
}
