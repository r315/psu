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
#include "graph.h"

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

typedef enum {MODEST_NORMAL = 0, MODEST_SET_V, MODEST_SET_I, MODEST_SET_SHOW} modestate_t;

typedef struct _calibration_t{
    uint16_t min;
    uint16_t max;
    uint16_t start;
}calibration_t;

typedef struct _State{ 
    uint8_t mode;
    uint8_t output_en;
    union{
        //uint32_t adcvalues[ADC_SEQ_LEN];
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
    uint8_t digit;
    int8_t base_place;
    modestate_t mode_state;
    uint8_t count;
    void selectDigit(int8_t sel);
    void changeDigit(int8_t base);
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

    virtual void init(){}
};

class ScreenPsu: public Screen{
    float set_v;
    float set_i;
    Graph graph;
    void printVoltage(float value, int8_t hide_place);
    void printCurrent(float value, int8_t hide_place);
    void printPower(float v, float i);
public:
    ScreenPsu() : Screen() {}	
    void process(State *st);
    void redraw();
    void modeSet();
    void initPreSetValues(float v_set, float i_set);
    void init();
};

class ScreenLoad: public Screen{
public:
    ScreenLoad() : Screen(){}	
    void process(State *st);
    void redraw();
    void modeSet();
    void init();
};

class ScreenCharger: public Screen{
    uint8_t bt_size;
public:
    ScreenCharger() : Screen(){}	
    void process(State *st);
    void redraw();
    void modeSet();
    void init();
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
    void init();
};

extern const uint8_t icon_out[];
extern const uint8_t icon_psu[];
extern const uint8_t icon_load[];
extern const uint8_t icon_chr[];
extern const uint8_t dro_unit_v[];
extern const uint8_t dro_unit_a[];

/**
 * @brief PSU control API
 * */

/**
 * @brief Set/Get output enable status
 * */
void psu_setOutputEnable(uint8_t en);
uint8_t psu_getOutputEnable(void);

/**
 * @brief Set/Get PSU output voltage
 * */
void psu_setOutputVoltage(float val, float max, float min);

/**
 * @brief Set/Get PSU output current
 * */
void psu_setOutputCurrent(float val, float max, float min);


#ifdef __cplusplus
}
#endif

#endif