
#include <string.h>
#include "bui.h"
#include "wi_dro.h"

#define BLINK_TIME_MASK             8


BUIdro::BUIdro(uint16_t x = 0, uint16_t y = 0) : BUIText(x ,y){
    setFont(&GroteskBold16x32);
    setPal((const uint16_t []){0x18E3, 0xEF58});    
}

void BUIdro::init(uint32_t base_pow, uint32_t init_pow, int32_t min, int32_t max, void (*format)(char *, int32_t)){
    _pow = init_pow;
    _min_pow = base_pow;
    _max_pow = base_pow * 100;
    _min_value = min;
    _max_value = max;
    _formater = format;
    // widget is visible but not invalid, will not be drawn until updated
}

void BUIdro::setValue(int32_t newvalue){
    _value = newvalue;
    update();    
}

void BUIdro::update(void){
    if(_formater != NULL){
        if(_text == NULL){
            setText("     ");  //force memory allocation
        }
        _formater(_text, _value);
        setInvalid(true);
    }
}

/**
 * @brief Enter/Exit edit value mode
 * 
 * \param dig : 0 exit edit mode
 *              1 select next left digit
 *             -1 select next right digit
 * */
void BUIdro::editValue(int8_t dig){
    if(dig == 0){        
        clrFlag(BUI_FLAG_EDIT);
        return;
    }

    if(!isFlagSet(BUI_FLAG_EDIT)){
        setFlag(BUI_FLAG_EDIT);
        _pow = _min_pow;
    }

    if(dig < 0){
        if(_pow > _min_pow) 
            _pow /= 10;
    }else{
        if(_pow < _max_pow)
            _pow *= 10;
    }
}

/**
 * @brief Change current selected value digit
 * \param dt : change amount
 * */
void BUIdro::changeValue(int8_t dt){        
    int32_t tmp = _value + (dt < 0 ?  -_pow : _pow);

    if(tmp <= _max_value && tmp >= _min_value){
        _value = tmp;
        update();
    }
}

void BUIdro::draw(void){
    if(isInvalid() || isFlagSet(BUI_FLAG_EDIT)){
        if(isVisible()){            
            if(isFlagSet(BUI_FLAG_EDIT) && (++_count) & BLINK_TIME_MASK){
                uint32_t cur_dig = _min_pow;  // Right most digit
                uint8_t blink_digit = 3;      // is located on index 3 of the string

                while(cur_dig < _pow){
                    blink_digit--;
                    if(_text[blink_digit] == '.'){
                        blink_digit--;
                    }
                    cur_dig *= 10;
                }

                uint16_t x = _x;
                for(uint16_t i = 0; i < _len; i++){
                    if(i == blink_digit){
                        x = drawChar(x, _y, ' ', _font, _pal);
                    }else{
                        x = drawChar(x, _y, _text[i], _font, _pal);
                    }
                }
            }else{
                drawString(_x, _y, _text, _font, _pal);
            }            
        }else{
            uint16_t x = _x;
            for(uint16_t i = 0; i < _len; i++){
                x = drawChar(x, _y, ' ', _font, _pal);            
            }
        }
        setInvalid(false);
    }
}