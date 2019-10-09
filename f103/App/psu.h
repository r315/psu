#ifndef _psu_h_
#define _psu_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "text.h"
#include <console.h>
#include "cmdhelp.h"
#include "cmdadc.h"
#include "cmdpwm.h"
#include "cmddfu.h"
#include "cmdpwr.h"
#include "cmdio.h"
#include "FreeRTOS.h"
#include "task.h"

#define ADC_INTERVAL 100
#define APP_INTERVAL 10
#define UPDATE_INTERVAL 100

#define VOLTAGE_DRO_POS             0,12
#define CURRENT_DRO_POS             69,12 
#define LOAD_ICON_POS               70,0
#define PSU_ICON_POS                90,0
#define OUTPUT_ICON_POS             110,0
#define MODE_ICONS_AREA_POS_S       70,0
#define MODE_ICONS_AREA_SIZE        40,8

#define MAX_SCREENS sizeof(screens)/sizeof(void*)

#define BLINK_TIME_MASK 8

static const uint16_t pwm_start_values [] = { 0x80, 0x180, 0x280, 0x380};

class Screen{
public:
    Screen() {}
    virtual void redraw(){}
    virtual void process(){}
    virtual void modeSet(){}
};

class ModePsu: public Screen{
    double increment;
    double set_v;
    double set_a;
    double *set_value;
    uint8_t mode_set;
    uint8_t count;
    
public:
    ModePsu() : Screen() {}	
    void process();
    void redraw();
    void modeSet();
};

class ModeLoad: public Screen{
public:
    ModeLoad() : Screen(){}	
    void process(){}
    void redraw();
    void modeSet(){}
};

typedef struct _Input{
    uint8_t value;
    uint8_t last_value;
}Input;

typedef struct _State{
    Input input; 
    uint8_t mode_select;
    uint8_t output_en;
    Screen *screen;
}State;

void toggleOutput(void);
void setOutput(uint8_t en);
void setMode(uint8_t mode);
void cycleMode(void);

extern const uint8_t icon_out[];
extern const uint8_t icon_psu[];
extern const uint8_t icon_load[];

State psu_state;

#ifdef __cplusplus
}
#endif

#endif