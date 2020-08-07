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
#include "cmdset.h"
#include "cmdstatus.h"
#include "FreeRTOS.h"
#include "task.h"
#include "graph.h"
#include "nvdata.h"
#include "dbg.h"

#define PRIORITY_LOW                3

#define VOLTAGE_PRECISION           0.005f  //MAX_VOLTAGE / 2^ADC_RESOLUTION
#define CURRENT_PRECISION           0.0008f
#define MAX_VOLTAGE                 14.0f
#define MIN_VOLTAGE                 1.2f
#define MAX_CURRENT                 3.00f
#define MIN_CURRENT                 0.00f
#define MAX_LOAD                    3.00f
#define MIN_LOAD                    0.00f

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

#define MAX_PRESETS                 6

#define EEPROM_ID_OFFSET            
#define EEPROM_PRESETS_OFFSET
#define EEPROM_PWM_CAL_OFFSET            

#if defined(ENABLE_DEBUG)
    #define DBG_PRINT dbg_printf
    #define DBG_DUMP_LINE dbg_HexDumpLine
#else
    #define DBG_PRINT(...)
    #define DBG_DUMP_LINE(...)
#endif

#define _FLAGS              psu.flags
#define SET_FLAG(f)         _FLAGS |= (1 << f)
#define CLR_FLAG(f)         _FLAGS &= ~(1 << f)
#define GET_FLAG(f)         ((_FLAGS & (1 << f)) != 0)
#define IS_FLAG_SET(x,f)    ((x & (1 << f)) != 0)
// Psu output enable flag
#define SET_OE_FLAG         SET_FLAG(0)
#define CLR_OE_FLAG         CLR_FLAG(0)
#define GET_OE_FLAG         GET_FLAG(0)

//Psu adc conversion ready
#define SET_AD_FLAG         SET_FLAG(1)
#define CLR_AD_FLAG         CLR_FLAG(1)
#define GET_AD_FLAG         GET_FLAG(1)


typedef enum {
    MODEST_NORMAL = 0, 
    MODEST_SET_V, 
    MODEST_SET_I, 
    MODEST_IDLE,
    MODEST_SAVE
}screenstate_e;

typedef struct pwmcal{
    uint16_t min;
    uint16_t max;
    uint16_t init;
}pwmcal_t;

typedef struct preset{
    float v;
    float i;
}preset_t;

typedef struct psu{ 
    pwmcal_t *pwm_cal;
    preset_t *preset;
    uint16_t rs;
    uint8_t cur_mode;
    volatile uint8_t flags;
    void *ptr;
}psu_t;

class Screen{

protected:
    float *set_value;
    float set_max;
    float set_min;
    uint8_t digit;
    int8_t base_place;
    screenstate_e _screen_state;
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
     * called every 100ms to process data 
     * */
    virtual void process(){}

    virtual void init(){}
};

class ScreenPsu: public Screen{
private:    
    float set_v;
    float set_i;
    Graph graph;
    void printVoltage(float value, int8_t hide_place);
    void printCurrent(float value, int8_t hide_place);
    void printPower(float v, float i);
    void printPresetIndex();
    void enterModeSet();
public:
    ScreenPsu() : Screen() {}	
    void process();
    void redraw();
    void enterModeSet();
    void init();
};

class ScreenLoad: public Screen{
public:
    ScreenLoad() : Screen(){}	
    void process();
    void redraw();
    void init();
};

class ScreenCharger: public Screen{
    uint8_t bt_size;
public:
    ScreenCharger() : Screen(){}	
    void process();
    void redraw();
    void init();
};

class ScreenPreset: public Screen{
private:
    int8_t _selected;
    preset_t *_presets;
    void selectPreset(uint8_t idx);
    void moveSelect(int8_t dir);
public:
    ScreenPreset() : Screen(), _selected(0) {}	
    void process();
    void redraw();
    void init();
};

extern const uint8_t icon_out[];
extern const uint8_t icon_psu[];
extern const uint8_t icon_load[];
extern const uint8_t icon_chr[];
extern const uint8_t dro_unit_v[];
extern const uint8_t dro_unit_a[];

extern char gOut[];

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
void psu_setOutputVoltage(float val);
float psu_getVoltage(void);

/**
 * @brief Set/Get PSU output current
 * */
void psu_setOutputCurrent(float val);
float psu_getCurrent(void);

/**
 * @brief
 * */
float psu_getLoadCurrent(void);

/**
 * @brief
 * */
uint8_t psu_getOutputEnable(void);

/**
 * @brief
 * */
uint8_t psu_AdcReady(void);

/**
 * @brief
 * */
void app_selectMode(uint8_t mode);

/**
 * @brief
 * */
preset_t *app_getPreset(void);
preset_t *app_getPresetList(void);
void app_setPreset(preset_t *preset);

#ifdef __cplusplus
}
#endif

#endif