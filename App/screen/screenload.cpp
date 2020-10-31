#include <strfunc.h>
#include "psu.h"
#include "draw.h"

#define LOAD_INFO_X         108
#define LOAD_INFO1_POS      LOAD_INFO_X,0
#define LOAD_INFO2_POS      LOAD_INFO_X,16
#define LOAD_INFO3_POS      LOAD_INFO_X,32
#define LOAD_INFO4_POS      LOAD_INFO_X,48
#define LOAD_INFO5_POS      LOAD_INFO_X,64
#define LOAD_POWER_POS      0,0
#define LOAD_MODE_POS       LOAD_INFO_X-32, 0

#define LOAD_GRAPH_X        1
#define LOAD_GRAPH_Y        (LCD_H - LOAD_GRAPH_H - 2)
#define LOAD_GRAPH_POS      LOAD_GRAPH_X,LOAD_GRAPH_Y
#define LOAD_GRAPH_H        40
#define LOAD_GRAPH_W        100

#define LOAD_LINE_COLOR     RGB565(5,10,5)

#define LOAD_TEXT_FONT      &courierFont

static const uint16_t graph_pal[] = {RGB565(5,10,5), RED, GREEN, YELLOW};
static const char *load_mode_name[] = {"CC", "CP", "CR"};

void ScreenLoad::init(){
    _graph.init(LOAD_GRAPH_POS, LOAD_GRAPH_W, LOAD_GRAPH_H, graph_pal);
    _screen_state = psu_getLoadEnabled() ? SCR_MODE_NORMAL : SCR_MODE_IDLE;
    _load_mode = LOAD_MODE_CC;
    _set_i = 0.1f;
    redraw();
}

void ScreenLoad::redraw(void){
    DRAW_FillRect(0, 0, LCD_W, LCD_H, BLACK);

    DRAW_HLine(0, 15, LCD_W, LOAD_LINE_COLOR);
    DRAW_VLine(LOAD_INFO_X - 3, 0, LCD_H, LOAD_LINE_COLOR);

    printMode(false);
    TEXT_SetPalette((const uint16_t[]){BLACK, PINK});
    updatePower(0);
    updateCurrent(BLINK_OFF, _set_i);
    updateVoltage(BLINK_OFF, 0);
    
    _graph.redraw();
}

void ScreenLoad::printMode(int8_t toggle_visible){
    const char *str = "  ";

    if(toggle_visible == false || (++_count) & BLINK_TIME_MASK){
        str = load_mode_name[(uint8_t)_load_mode];
    }
    
    TEXT_SetPalette((const uint16_t[]){BLACK, DARKORANGE});
    TEXT_Print(LOAD_MODE_POS, str);    
}

void ScreenLoad::updateCurrent(uint8_t hide_digit, uint32_t ma){
    printCurrent(LOAD_INFO2_POS, LOAD_TEXT_FONT, (const uint16_t[]){BLACK, YELLOW}, hide_digit, ma);
}

void ScreenLoad::updateVoltage(uint8_t hide_digit, uint32_t mv){
    printVoltage(LOAD_INFO3_POS, LOAD_TEXT_FONT, (const uint16_t[]){BLACK, GREEN}, hide_digit, mv);
}

void ScreenLoad::updatePower(uint32_t pwr){
    printPower(LOAD_POWER_POS, LOAD_TEXT_FONT, (const uint16_t[]){BLACK, SKYBLUE}, BLINK_OFF, pwr);
}

void ScreenLoad::updateTime(){
    uint32_t elapsed_ms = ElapsedTicks(_start_ticks);
    uint32_t hour = elapsed_ms / 3600000;
    uint32_t min = (elapsed_ms / 60000) % 60;

    xsprintf(gOut,"%02u:%02u", hour, min);
    TEXT_Print(LOAD_INFO4_POS, gOut);
}

void ScreenLoad::process(){
uint32_t i, v;
static uint8_t blink;

    switch(_screen_state){
        case SCR_MODE_IDLE:
            if(BUTTON_GetEvents() == BUTTON_PRESSED){
                switch(BUTTON_VALUE){
                    case BUTTON_MEM: 
                        psu_setLoadEnable(true); 
                        _screen_state = SCR_MODE_NORMAL;
                        _start_ticks = GetTicks();
                        break;
                    case BUTTON_SET:
                        _screen_state = SCR_MODE_SET_MODE;
                        break;
                }
            }
            break;

        case SCR_MODE_NORMAL:
            if(BUTTON_GetEvents() == BUTTON_PRESSED){
                switch(BUTTON_VALUE){
                    case BUTTON_MEM: 
                        // disable load and show set points
                        psu_setLoadEnable(false);
                        updateCurrent(BLINK_OFF, _set_i);
                        _screen_state = SCR_MODE_IDLE;
                        return;

                    case BUTTON_SET:
                        _screen_state = SCR_MODE_SET_MODE;
                        break;
                    default: break;
                }
            }

            // update display
            i = psu_getLoadCurrent();
            v = psu_getVoltage();

            updateCurrent(BLINK_OFF, i);
            updateVoltage(BLINK_OFF, v);
            updatePower(v * i);
            updateTime();

            _graph.addPoint((int)v, 0);
            _graph.addPoint((int)i, 1);
            _graph.nextPoint();
            _graph.update();                
            break;

        case SCR_MODE_SET_MODE:
            if(BUTTON_GetEvents() != BUTTON_PRESSED){
                printMode(true);
                break;
            }

            switch(BUTTON_VALUE){
                case BUTTON_SET:
                    switch(_load_mode){
                    case LOAD_MODE_CC: 
                        _screen_state = SCR_MODE_SET_CC;
                        _set_i = 40;
                        blink = BLINK_ON;
                        configSetting(10, 10, 1000, MIN_CURRENT, MAX_CURRENT);                       
                        break;
                    case LOAD_MODE_CP: _screen_state = SCR_MODE_SET_CP; break;
                    case LOAD_MODE_CR: _screen_state = SCR_MODE_SET_CR; break;
                    default: break;
                    }                
                    break;

                case BUTTON_RIGHT:
                    switch(_load_mode){
                    case LOAD_MODE_CC: _load_mode = LOAD_MODE_CP; break;
                    case LOAD_MODE_CP: _load_mode = LOAD_MODE_CR; break;
                    case LOAD_MODE_CR: _load_mode = LOAD_MODE_CC; break;
                    }
                    break;

                case BUTTON_LEFT:
                    switch(_load_mode){
                    case LOAD_MODE_CC: _load_mode = LOAD_MODE_CR; break;
                    case LOAD_MODE_CP: _load_mode = LOAD_MODE_CC; break;
                    case LOAD_MODE_CR: _load_mode = LOAD_MODE_CP; break;
                    }
                    break;
                    
                default:
                    break;
            }

            printMode(false);            
            break;

        case SCR_MODE_SET_CP:
        case SCR_MODE_SET_CR:
        case SCR_MODE_SET_CC:
            if(BUTTON_GetEvents() == BUTTON_PRESSED){
                switch(BUTTON_VALUE){
                    case BUTTON_SET:
                        // Exit CC set mode
                        if(psu_getLoadEnabled()){
                            _screen_state = SCR_MODE_NORMAL;
                        }else{
                            _screen_state = SCR_MODE_IDLE;
                        }
                        blink = BLINK_OFF;
                        psu_setLoadCurrent(_set_i);
                        break;

                    case BUTTON_UP: addPow(&_set_i); break;
                    case BUTTON_DOWN: subPow(&_set_i); break;
                    case BUTTON_LEFT: mulPow(); break;
                    case BUTTON_RIGHT: divPow(); break;
                }
            }

            updateCurrent(blink, _set_i);
            break;

        default: 
            break;
    }    
}