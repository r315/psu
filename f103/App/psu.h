#ifndef _psu_h_
#define _psu_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "layout_ui.h"
#include "text.h"
#include <console.h>
#include "cmdhelp.h"
#include "cmdadc.h"
#include "cmdpwm.h"
#include "cmddfu.h"
#include "cmdpwr.h"
#include "FreeRTOS.h"
#include "task.h"

#define ADC_INTERVAL 100
#define APP_INTERVAL 10

static const uint16_t pwm_start_values [] = { 0x80, 0x180, 0x280, 0x380};
enum Mode {PSU, LOAD};

typedef struct _Input{
    uint8_t value;
    uint8_t last_value;
}Input;

class Screen{

public: 
    void process(Input inp);
};


typedef struct _State{
    Input input;
    Mode mode;
    Screen screen;
}State;

#ifdef __cplusplus
}
#endif

#endif