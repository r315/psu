#ifndef _psu_h_
#define _psu_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "board.h"

#define PSU_VERSION_MAJOR           0
#define PSU_VERSION_MINOR           0
#define PSU_VERSIO_PATCH            0

#define PRIORITY_LOW                3

#define MAX_VOLTAGE                 20000U
#define MIN_VOLTAGE                 1300U
#define MAX_CURRENT                 3000U
#define MIN_CURRENT                 0U
#define MAX_ILOAD                   1000U  // TODO: LIMIT in HW to 500mA
#define MIN_ILOAD                   0U
#define MAX_VLOAD                   20000U
#define MIN_VLOAD                   0U

#define ADC_INTERVAL                100 //ms
#define APP_INTERVAL                10
#define UPDATE_INTERVAL             100

#define MAX_CELL_VOLTAGE            4190

#define CONSOLE_PROMPT              "PSU >"

#define MAX_PRESETS                 6   

#define WATCHDOG_TIME               3000U   // ms
#define POWER_OFF_COUNT             20      // Iterations before calling power off
#define POWER_OFF_DELAY             700     // ms Delay before setting HW off

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
    float an_channel_gain[AN_MUX_NUM_CH];
    pwmcal_t pwm_cal[PWM_NUM_CH];
    preset_t preset_list[MAX_PRESETS];
    uint8_t preset_idx;                 
    uint8_t cksum;
    volatile uint8_t flags;         // Above fields are saved to eeprom
    uint16_t *adc_data;
}psu_t;

/**
 * @brief Global adc conversions counter
 * */
extern volatile uint32_t g_mgr_eoc_count;

/**
 * @brief PSU control API
 * */

/**
 * @brief Get channel voltage
 * 
 * \param channel : Channel 
 * \return : voltage in mv
 * */
uint32_t psu_getChannelVoltage(uint8_t channel);

/**
 * @brief Get cell voltage at balancer terminals
 * \param c : cell number [4-1], counting from gnd
 * \return : cell voltage in mV
 * */
uint32_t psu_getCellVoltage(uint8_t c);

/**
 * @brief Set/Get output enable status
 * */
uint8_t app_isOutputEnabled(void);
void app_setOutputEnable(uint8_t en);
uint8_t app_toggleOutputEnable(void);
void psu_setOutputEnable(uint8_t en);

/**
 * @brief Set/Get PSU output voltage
 * */
uint32_t psu_getOutputVoltage(void);
void psu_setOutputVoltage(uint32_t mv);

/**
 * @brief Set/Get PSU output current
 * */
uint32_t psu_getOutputCurrent(void);
void psu_setOutputCurrent(uint32_t ma);


/**
 * @brief Get Load voltage
 * */
uint32_t psu_getLoadVoltage(void);

/**
 * @brief Set/Get Application Load Enable
 * */
uint8_t app_isLoadEnabled(void);
void app_setLoadEnable(uint8_t en);

/**
 * @brief Set/Get PSU Load Current
 * */
uint32_t psu_getLoadCurrent(void);
void psu_setLoadCurrent(uint32_t ma);

/**
 * @brief Get USB port current
 * */
uint32_t psu_getUsbCurrent(void);

/**
 * @brief Get input voltage at 
 * Buck converter
 * */
uint32_t psu_getInputVoltage(void);

/**
 * @brief
 * */
uint8_t app_isAdcDone(void);

/**
 * @brief Get list of all presets
 * 
 * \return pointer to preset array
 * */
preset_t *app_getPresetList(void);
/**
 * @brief Get/Set last used preset index
 * */
uint8_t app_getSavedPresetIdx(void);
void app_setPresetIdx(uint8_t idx);

/**
 * @brief Set psu output to values given by preset
 * 
 * \param preset : preset with values
 * */
void app_applyPreset(preset_t *preset);

/**
 * @brief Set psu output from preset values at given index
 * 
 * \param idx : new index
 * */
void app_applyPresetByIdx(uint8_t idx);

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

/**
 * @brief
 * */
void psu_poweroff(void);

/**
 * @brief
 * */
float psu_getChannelGain(uint8_t ch);
void psu_setChannelGain(uint8_t ch, float g);

/**
 * @brief
 * */
void psu_setPwmChannelCalibration(uint8_t ch, uint16_t min, uint16_t max);
pwmcal_t psu_getPwmChannelCalibration(uint8_t ch);

#ifdef __cplusplus
}
#endif

#endif