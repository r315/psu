#include "psu.h"
#include "draw.h"

#define LOAD_INFO_X         108
#define LOAD_INFO1_POS      LOAD_INFO_X,0
#define LOAD_INFO2_POS      LOAD_INFO_X,16
#define LOAD_INFO3_POS      LOAD_INFO_X,32
#define LOAD_INFO4_POS      LOAD_INFO_X,48
#define LOAD_INFO5_POS      LOAD_INFO_X,64

#define LOAD_GRAPH_X        1
#define LOAD_GRAPH_Y        (LCD_H - LOAD_GRAPH_H - 2)
#define LOAD_GRAPH_POS      LOAD_GRAPH_X,LOAD_GRAPH_Y
#define LOAD_GRAPH_H        40
#define LOAD_GRAPH_W        100

#define LOAD_LINE_COLOR     RGB565(5,10,5)

static const uint16_t txt_pal[] = {BLACK, WHITE};
static const uint16_t graph_pal[] = {RGB565(5,10,5), RED, GREEN, YELLOW};
static const char *load_mode_name[] = {"CC", "CP", "CR"};

void ScreenLoad::printMode(int8_t toggle_visible){
    static uint8_t count = 0;

    if(toggle_visible == false || (++count) & BLINK_TIME_MASK){
        TEXT_Print(LOAD_INFO1_POS, load_mode_name[(uint8_t)_load_mode]);   
    }else {
        TEXT_Print(LOAD_INFO1_POS, "  ");
    }
}


void ScreenLoad::init(){
    _graph.init(LOAD_GRAPH_POS, LOAD_GRAPH_W, LOAD_GRAPH_H, graph_pal);
    _screen_state = SCR_MODE_IDLE;
    _load_mode = LOAD_MODE_CC;
    redraw();
}

void ScreenLoad::redraw(void){
    DRAW_FillRect(0, 0, LCD_W, LCD_H, BLACK);

    DRAW_HLine(0, 15, LCD_W, LOAD_LINE_COLOR);
    DRAW_VLine(LOAD_INFO_X - 3, 0, LCD_H, LOAD_LINE_COLOR);

    TEXT_SetPalette(txt_pal);
    printMode(false);
    TEXT_Print(LOAD_INFO2_POS,"00.0W");
    TEXT_Print(LOAD_INFO3_POS,"0.50A");
    TEXT_Print(LOAD_INFO4_POS,"4.21V");
    TEXT_Print(LOAD_INFO5_POS,"00:00");

    _graph.redraw();
}

bool checkModeChange(){
    return (BUTTON_GetEvents() == BUTTON_PRESSED) && (BUTTON_VALUE == BUTTON_SET);
}

void ScreenLoad::process(){
    bool toggle_visible;
    switch(_screen_state){
        case SCR_MODE_IDLE:
        case SCR_MODE_NORMAL:
            if(checkModeChange()){
                _screen_state = SCR_MODE_SET_MODE;
            }
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
                            break;

                        case LOAD_MODE_CP:
                            _screen_state = SCR_MODE_SET_CP;
                            break;

                        case LOAD_MODE_CR:
                            _screen_state = SCR_MODE_SET_CR;
                            break;

                        default:
                            break;
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
            }

            printMode(false);            
            break;

        case SCR_MODE_SET_CC:
        case SCR_MODE_SET_CP:
        case SCR_MODE_SET_CR:
            if(checkModeChange()){
                _screen_state = SCR_MODE_NORMAL;
                
            }
            break;
    }    
}