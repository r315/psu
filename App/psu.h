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

#define LOAD_ICON_POS               70,0
#define OUTPUT_ICON_POS             160-16,0
#define MODE_ICONS_AREA_POS_S       70,0
#define MODE_ICONS_AREA_SIZE        40,8

#define MAX_MODES sizeof(modes)/sizeof(void*)
#define SET_MAX_DIGITS              3 // Digits in each DRO

#define BLINK_TIME_MASK             8
#define NO_BLANK                    -1

#define CONSOLE_PROMPT              "PSU >"
#define STATE_FLAG_DISPLAY          1   // LCD Init flag

#define MAX_PRESET                  6

enum SET_MODE {SET_OFF = 0, SET_M1, SET_M2};

typedef struct _calibration_t{
    uint16_t min;
    uint16_t max;
    uint16_t start;
}calibration_t;

typedef struct _State{ 
    uint8_t mode;
    uint8_t output_en;
    union{
        uint32_t adcvalues[ADC_SEQ_LEN];
        struct {
            uint16_t adc_v1;
            uint16_t adc_i1;
            uint16_t adc_v2;
            uint16_t adc_i2;
            uint16_t adc_v3;
            uint16_t adc_i3;
            uint16_t adc_v4;
            uint16_t adc_i4;
            uint16_t adc_vl;
            uint16_t adc_il;    
        };
    };
    uint8_t flags;
    calibration_t cal_data[PWM_NUM_CH];
    void *ctx;
}State;

class Screen{

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
    Screen() {}
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
};

class ScreenPsu: public Screen{
    float set_v;
    float set_i;

    void printVoltage(float value, int8_t hide_place);
    void printCurrent(float value, int8_t hide_place);
    void printPower(float v, float i);
public:
    ScreenPsu() : Screen() {}	
    void process(State *st);
    void redraw();
    void modeSet();
    void startValues(float v_last, float i_last);
};

class ScreenLoad: public Screen{
public:
    ScreenLoad() : Screen(){}	
    void process(State *st);
    void redraw();
    void modeSet();
};

class ScreenCharger: public Screen{
    uint8_t bt_size;
public:
    ScreenCharger() : Screen(){}	
    void process(State *st);
    void redraw();
    void modeSet();
};

typedef struct preset{
    float voltage;
    float current;
}preset_t;

class ScreenPreset: public Screen{
private:
    uint8_t selected;
    preset_t presets[MAX_PRESET];
    void createPreset(uint16_t idx, uint16_t *buf);
public:
    ScreenPreset() : Screen(){}	
    void process(State *st);
    void redraw();
    void modeSet();
};

void app_setOutputEnable(uint8_t en);
void app_setOutputVoltage(float val, float max, float min);
void app_setOutputCurrent(float val, float max, float min);

extern const uint8_t icon_out[];
extern const uint8_t icon_psu[];
extern const uint8_t icon_load[];
extern const uint8_t icon_chr[];
extern const uint8_t dro_unit_v[];
extern const uint8_t dro_unit_a[];

#ifdef __cplusplus
}
#endif

#endif