#ifndef _psu_h_
#define _psu_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

#include "board.h"
#include "text.h"

#ifdef __cplusplus
#include "graph.h"
}
#endif

#define PSU_VERSION_MAJOR           0
#define PSU_VERSION_MINOR           0
#define PSU_VERSIO_PATCH            0

#define PRIORITY_LOW                3

#define MAX_VOLTAGE                 20000U
#define MIN_VOLTAGE                 1200U
#define MAX_CURRENT                 3000U
#define MIN_CURRENT                 0U
#define MAX_LOAD                    MAX_CURRENT
#define MIN_LOAD                    MIN_CURRENT

#define VOUT_MUX_CH                 0
#define IOUT_MUX_CH                 1
#define VLOAD_MUX_CH                2
#define ILOAD_MUX_CH                3
#define VIN_MUX_CH                  4
#define IUSB_MUX_CH                 13

#define ADC_INTERVAL                100 //ms
#define APP_INTERVAL                10
#define UPDATE_INTERVAL             100

#define LOAD_ICON_POS               LCD_W - 36,3
#define OUTPUT_ICON_POS             160-16,3

#define SCREEN_NUM                   sizeof(screens)/sizeof(void*)

#define BLINK_TIME_MASK             8
#define BLINK_ON                    1
#define BLINK_OFF                   0

#define CONSOLE_PROMPT              "PSU >"
//#define STATE_FLAG_DISPLAY          1   // LCD Init flag

#define MAX_PRESETS                 6

#define EEPROM_ID_OFFSET            
#define EEPROM_PRESETS_OFFSET
#define EEPROM_PWM_CAL_OFFSET            

#define WATCHDOG_TIME               3000U   // ms
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

// EEPROM load result
#define SET_EEPROM_FLAG     SET_FLAG(4)
#define CLR_EEPROM_FLAG     CLR_FLAG(4)
#define GET_EEPROM_FLAG     GET_FLAG(4)


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
    uint8_t preset_idx;                 
    uint8_t screen_idx;
    preset_t preset_list[MAX_PRESETS];
    pwmcal_t pwm_cal[PWM_NUM_CH];
    float an_channel_gain[AN_MUX_NUM_CH];
    uint8_t cksum;
    volatile uint8_t flags;         // Above fields are saved to eeprom
    uint16_t *adc_data;
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
uint32_t psu_getVin(void);


/**
 * @brief Get channel voltage
 * 
 * \param channel : Channel 
 * \return : voltage in mv
 * */
uint32_t psu_getChannelVoltage(uint8_t channel);

/**
 * @brief Set/Get PSU output current
 * */
void psu_setOutputCurrent(uint32_t ma);
uint32_t psu_getCurrent(void);
uint32_t psu_getCurrentUSB(void);

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
 * @brief Change screen
 * 
 * \param screen_idx : new screen index
 * */
void app_selectScreen(uint8_t screen_idx);

/**
 * @brief Get current preset
 * 
 * \return : preset_t with current and voltage setting
 * */
preset_t app_getPreset(void);

/** 
 * @brief Get current preset index
 * 
 * \return : index
 * */
uint8_t app_getPresetIdx(void);

/**
 * @brief Get list of all presets
 * 
 * \return pointer to preset array
 * */
preset_t *app_getPresetList(void);

/**
 * @brief Update current preset
 * 
 * \param preset : preset with new values
 * */

void app_setPreset(preset_t preset);
/**
 * @brief Change current preset using index
 * 
 * \param idx : new index
 * */
void app_setPresetIdx(uint8_t idx);

/**
 * @brief enables/disables adc manager
 * Use for testing, inibits/allow tsk_psu of calling ADCMGR_Start
 * */
void app_enable_adcmgr(uint8_t en);

/**
 * @brief Saves app state to eeprom
 * 
 * The value of presets, pwm calibration, adc gains, current preset selected...
 * are saved to eeprom. 
 * 
 * \return : 1 if successful, 0 other wise
 * */
uint8_t app_saveState(void);


/**
 * @brief Restore app state from eeprom
 * 
 * The value of presets, pwm calibration, adc gains, current preset selected...
 * are restored from eeprom. 
 * 
 * \return : 1 if successful, 0 loaded default values
 * */
uint8_t app_restoreState(void);


/**
 * @brief Restore app default state
 * 
 * The value of presets, pwm calibration, adc gains, current preset selected...
 * are set to defaults
 * 
 * */
void app_defaultState(void);

#ifdef __cplusplus
}
#endif

#endif