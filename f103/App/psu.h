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
#include "cmdout.h"
#include "FreeRTOS.h"
#include "task.h"

#define ADC_INTERVAL                100 //ms
#define APP_INTERVAL                10
#define UPDATE_INTERVAL             100

#define VOLTAGE_DRO_POS             0,12 // (x,y)
#define CURRENT_DRO_POS             69,12 
#define LOAD_ICON_POS               70,0
#define PSU_ICON_POS                90,0
#define OUTPUT_ICON_POS             110,0
#define MODE_ICONS_AREA_POS_S       70,0
#define MODE_ICONS_AREA_SIZE        40,8

#define MAX_MODES sizeof(modes)/sizeof(void*)
#define SET_MAX_DIGITS              3 // Digits in each DRO

#define BLINK_TIME_MASK             8
#define NO_BLANK                    -1

#define CONSOLE_PROMPT              "PSU >"
#define STATE_FLAG_DISPLAY          1   // LCD Init flag

enum SET_MODE {SET_OFF = 0, SET_VOLTAGE, SET_CURRENT};

typedef struct _calibration_t{
    uint16_t max;
    uint16_t min;
    uint16_t start;
}calibration_t;

typedef struct _State{ 
    uint8_t mode_select;
    uint8_t output_en;
    union{
        uint64_t adcvalues;
        struct {
            uint16_t adc_out_v;
            uint16_t adc_out_i;
            uint16_t adc_in_v;
            uint16_t adc_in_i;    
        };
    };
    uint8_t flags;
    calibration_t cal_data[PWM_NUM_CH];
    void *ctx;
}State;

class Mode{

protected:
    float *set_value;
    float set_max;
    float set_min;
    uint8_t place;
    int8_t base_place;
    uint8_t mode_set;
    uint8_t count;
    uint16_t mapPwm(float x, float in_max);
    void changePlace(int8_t sel);
    void incrementPlace(int8_t base);
    void (*setOutput)(float val, float max, float min);
public:
    Mode() {}
    /**
     * Redraws the mode screen with default values
     * called when mode is selected
     * */
    virtual void redraw(){}
    /**
     * Enter mode configuration
     * called when user press the set key
     * */
    virtual void modeSet(){}
    /**
     * called every 100ms to process data 
     * */
    virtual void process(State *st){}
    /**
     * Shows power on a given location,
     * font must be set before calliing it
     * */
    void printPower(uint16_t x, uint16_t y, float v, float i);
    void printVoltage(uint16_t x, uint16_t y, float v);
};

class ModePsu: public Mode{
    float set_v;
    float set_i;        
public:
    ModePsu() : Mode() {}	
    void process(State *st);
    void redraw();
    void modeSet();
};

class ModeLoad: public Mode{
public:
    ModeLoad() : Mode(){}	
    void process(State *st);
    void redraw();
    void modeSet();
};



void toggleOutput(void);
void setOutputEnable(uint8_t en);
void setMode(uint8_t mode);
void cycleMode(void);
void setOutputVoltage(float val, float max, float min);
void setOutputCurrent(float val, float max, float min);

extern const uint8_t icon_out[];
extern const uint8_t icon_psu[];
extern const uint8_t icon_load[];

#ifdef __cplusplus
}
#endif

#endif