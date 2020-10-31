#ifndef _psu_h_
#define _psu_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "text.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
#include "graph.h"
}
#endif

#define PRIORITY_LOW                3

#define VOLTAGE_PRECISION           5U
#define CURRENT_PRECISION           8U
#define MAX_VOLTAGE                 20000U
#define MIN_VOLTAGE                 1200U
#define MAX_CURRENT                 3000U
#define MIN_CURRENT                 0U
#define MAX_LOAD                    MAX_CURRENT
#define MIN_LOAD                    MIN_CURRENT

#define ADC_INTERVAL                100 //ms
#define APP_INTERVAL                10
#define UPDATE_INTERVAL             100

#define LOAD_ICON_POS               LCD_W - 36,3
#define OUTPUT_ICON_POS             160-16,3

#define MAX_MODES                   sizeof(modes)/sizeof(void*)

#define BLINK_TIME_MASK             8
#define BLINK_ON                    1
#define BLINK_OFF                   0

#define CONSOLE_PROMPT              "PSU >"
//#define STATE_FLAG_DISPLAY          1   // LCD Init flag

#define MAX_PRESETS                 6

#define EEPROM_ID_OFFSET            
#define EEPROM_PRESETS_OFFSET
#define EEPROM_PWM_CAL_OFFSET            

#define _FLAGS              psu.flags
#define SET_FLAG(f)         _FLAGS |= (1 << f)
#define CLR_FLAG(f)         _FLAGS &= ~(1 << f)
#define GET_FLAG(f)         ((_FLAGS & (1 << f)) != 0)
#define IS_FLAG_SET(x,f)    ((x & (1 << f)) != 0)
// Psu output enable flag
#define SET_OE_FLAG         SET_FLAG(0)
#define CLR_OE_FLAG         CLR_FLAG(0)
#define GET_OE_FLAG         GET_FLAG(0)

// Psu adc conversion ready
#define SET_AD_FLAG         SET_FLAG(1)
#define CLR_AD_FLAG         CLR_FLAG(1)
#define GET_AD_FLAG         GET_FLAG(1)

// Dummy load enable flag
#define SET_LD_FLAG         SET_FLAG(2)
#define CLR_LD_FLAG         CLR_FLAG(2)
#define GET_LD_FLAG         GET_FLAG(2)

// ADC Manager enable
#define SET_ADCMGR_FLAG     SET_FLAG(3)
#define CLR_ADCMGR_FLAG     CLR_FLAG(3)
#define GET_ADCMGR_FLAG     GET_FLAG(3)


#ifndef FALSE
#define FALSE   (0)
#endif
#ifndef TRUE
#define TRUE    (1)
#endif
#ifndef OFF
#define OFF   (0)
#endif

#ifndef ON
#define ON    (1)
#endif

typedef enum {
    SCR_MODE_IDLE = 0,
    SCR_MODE_NORMAL,
    SCR_MODE_SET_V, 
    SCR_MODE_SET_I, 
    SCR_MODE_SAVE,
    SCR_MODE_SET_MODE,
    SCR_MODE_SET_CC,
    SCR_MODE_SET_CP,
    SCR_MODE_SET_CR,
}screenstate_e;

typedef struct pwmcal{
    uint16_t min;
    uint16_t max;
    uint16_t init;
}pwmcal_t;

typedef struct preset{
    uint32_t v;
    uint32_t i;
}preset_t;

typedef struct psu{ 
    pwmcal_t *pwm_cal;
    preset_t *preset;
    uint16_t rs;
    uint8_t cur_mode;
    volatile uint8_t flags;
    void *ptr;
}psu_t;

typedef enum {
    LOAD_MODE_CC = 0,
    LOAD_MODE_CP,
    LOAD_MODE_CR
}loadmode_e;

#ifdef __cplusplus
extern "C" {

class Screen{

private:
    void _addPow(uint32_t *value, int32_t pow);
    void _printValue(uint16_t x, uint16_t y, font_t *font, const uint16_t *pal, uint8_t blink_digit, const char *str);

protected:
    screenstate_e _screen_state;
    uint8_t _count;
    uint32_t _pow, _min_pow, _max_pow;
    int32_t _min_set;
    int32_t _max_set;
    void divPow(void);
    void mulPow(void);
    void addPow(uint32_t *val);
    void subPow(uint32_t *val);

    void configSetting(uint32_t initial_pow, uint32_t min_pow, uint32_t max_pow, int32_t min_set, int32_t max_set){
        _pow = initial_pow; _min_pow = min_pow; _max_pow = max_pow;
        _min_set = min_set; _max_set = max_set;
    }

    void printCurrent(uint16_t x, uint16_t y, font_t *font, const uint16_t *pal, uint8_t blink_digit, uint32_t ma);
    void printVoltage(uint16_t x, uint16_t y, font_t *font, const uint16_t *pal, uint8_t blink_digit, uint32_t mv);
    void printPower(uint16_t x, uint16_t y, font_t *font, const uint16_t *pal, uint8_t blink_digit, uint32_t mw);
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
    uint32_t _set_v;
    uint32_t _set_i;
    Graph _graph;
    void updateCurrent(uint8_t hide_digit, uint32_t ma);
    void updateVoltage(uint8_t hide_digit, uint32_t mv);
    void updatePower(uint32_t pwr);
    void updatePresetIndex(void);
public:
    ScreenPsu() : Screen() {}	
    void process();
    void redraw();
    void init();
};

class ScreenLoad: public Screen{
    Graph _graph;
    loadmode_e _load_mode;
    uint32_t _set_i;
    uint32_t _start_ticks;
    void printMode(int8_t toggle_visible);
    void updateCurrent(uint8_t hide_digit, uint32_t ma);
    void updateVoltage(uint8_t hide_digit, uint32_t mv);
    void updatePower(uint32_t pwr);
    void updateTime(void);
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
#endif /* __cplusplus */
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
void psu_setOutputVoltage(uint32_t mv);
uint32_t psu_getVoltage(void);

/**
 * @brief Set/Get PSU output current
 * */
void psu_setOutputCurrent(uint32_t ma);
uint32_t psu_getCurrent(void);

/**
 * @brief
 * */
void psu_setLoadEnable(uint8_t en);
uint8_t psu_getLoadEnabled(void);

/**
 * @brief
 * */
void psu_setLoadCurrent(uint32_t ma);
uint32_t psu_getLoadCurrent(void);

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