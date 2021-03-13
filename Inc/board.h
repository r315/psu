#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <button.h>
#include <stdout.h>
#include "stm32f1xx.h"
#include "pcf8574.h"
#include "lcd.h"
#include "st7735.h"
#include "adcmgr.h"
#include "pinName.h"
#include "dbg.h"
#include "eeprom.h"


/**
 * Button 
 * */
#define BUTTON_UP       (1<<2)
#define BUTTON_DOWN     (1<<3)
#define BUTTON_LEFT  	(1<<1)
#define BUTTON_RIGHT 	(1<<0)
#define BUTTON_SET  	(1<<4)
#define BUTTON_MODE     (1<<6)
#define BUTTON_OUT      (1<<5)
#define BUTTON_PRE      (1<<7)

#define BUTTON_HW_INIT
#define BUTTON_HW_READ (255 - EXPANDER_Read())
#define BUTTON_MASK (BUTTON_LEFT | BUTTON_RIGHT | BUTTON_UP   | BUTTON_DOWN |\
                     BUTTON_SET  | BUTTON_OUT   | BUTTON_MODE | BUTTON_MEM)
#define BUTTON_VALUE (uint8_t)BUTTON_GetValue()

/**
 * Softpower, PA1, PA2
 * 
 * PA1/ADC1 - Can measure voltage after DC jack
 * PA2 - Can detect if power key was pressed by reading ADC2.
 *       Seting this pin as digital output and writing low perform
 *       poweroff
 * */

#define PWR_BTN_PRESSED      0x300 // TODO: Fix voltage fluctuation on PA2 with different input voltages

#define PWR_BTN_PIN          PA_2

#define SOFT_POWER_OFF                     \
{                                          \
    pinWrite(PWR_BTN_PIN, GPIO_PIN_RESET); \
    pinInit(PWR_BTN_PIN, GPO_2MHZ);        \
    while(1);                              \
}

#define SPOWER_INIT                   \
{                                     \
    pinInit(PWR_BTN_PIN, GPI_ANALOG); \
}

#define GET_PWR_BTN         (ADC2_Read(ADC_PWR_SW_CH) > PWR_BTN_PRESSED)


/**
 * LEDS and debug pin
 * */
#define LED_PIN         PC_13
#define LED_INIT        pinInit(LED_PIN, GPO_2MHZ)
#define LED_TOGGLE      HAL_GPIO_TogglePin(PIN_NAME_TO_PORT(LED_PIN), PIN_NAME_TO_PIN(LED_PIN))
#define LED_OFF         pinWrite(LED_PIN, GPIO_PIN_SET)
#define LED_ON          pinWrite(LED_PIN, GPIO_PIN_RESET)

#define DBG_PIN         PB_6
#define DBG_PIN_INIT    pinInit(DBG_PIN, GPO_2MHZ);
#define DBG_PIN_HIGH    pinWrite(DBG_PIN, GPIO_PIN_SET)
#define DBG_PIN_LOW     pinWrite(DBG_PIN, GPIO_PIN_RESET)
#define DBG_PIN_TOGGLE  pinToggle(DBG_PIN)

#if defined(ENABLE_DEBUG)
    #define DBG_PRINT dbg_printf
    #define DBG_DUMP_LINE dbg_HexDumpLine
#else
    #define DBG_PRINT(...)
    #define DBG_DUMP_LINE(...)
#endif

/**
 * Output enable pin
 * */
#define PSU_OE_PIN      PB_1
#define PSU_OE_INIT     pinWrite(PSU_OE_PIN, GPIO_PIN_RESET); pinInit(PSU_OE_PIN, GPO_2MHZ)
#define PSU_OE_SET(_X)  pinWrite(PSU_OE_PIN, _X)

/**
 * Analog mux selectors
 * PA7-4
 * */
#define MUX_S0  PA_4
#define MUX_S1  PA_5
#define MUX_S2  PA_6
#define MUX_S3  PA_7

#define MUX_SEL_INIT \
{                                                                \
    pinWrite(MUX_S0, GPIO_PIN_RESET); pinInit(MUX_S0, GPO_2MHZ); \
    pinWrite(MUX_S1, GPIO_PIN_RESET); pinInit(MUX_S1, GPO_2MHZ); \
    pinWrite(MUX_S2, GPIO_PIN_RESET); pinInit(MUX_S2, GPO_2MHZ); \
    pinWrite(MUX_S3, GPIO_PIN_RESET); pinInit(MUX_S3, GPO_2MHZ); \
}

#define MUX_SELECT(CH) portWrite(PORTA, (portRead(PORTA) & ~(0xF<<4)) | (CH) << 4)

/**
 * Buzzer
 * */
#define BUZ_INIT

/**
 * Delay and tick count
 * */
#define GetTicks HAL_GetTick
#define DelayMs(d) HAL_Delay(d)

static inline uint32_t ElapsedTicks(uint32_t start_ticks){ 
    uint32_t current = GetTicks(); 
    //return (current > start_ticks) ? current - start_ticks : 0xFFFFFFFF - start_ticks + current;
    return current - start_ticks;
}

void TICK_Init(void);
/**
 * SPI
 * */
#define SPI_BLOCK_DMA

void SPI_Init(void);
uint8_t SPI_Send(uint8_t data);
void SPI_Read(uint8_t *dst, uint32_t len);
void SPI_WriteDMA(uint16_t *dst, uint32_t len);
/**
 * Display
 * PB3  -> BKL
 * PB12 -> CS
 * PB13 -> SCK
 * PB14 -> RS
 * PB15 -> SDO
 * */
#define TFT_W               80
#define TFT_H               160  // 162 on GRAM

#define TFT_OFFSET_SOURCE	26
#define TFT_OFFSET_GATE		1
#define TFT_BGR_FILTER

#define LCD_CD_Pin          GPIO_PIN_14
#define LCD_CD_GPIO_Port    GPIOB
#define LCD_BKL_Pin         GPIO_PIN_3
#define LCD_BKL_GPIO_Port   GPIOB
#define LCD_CS_Pin          GPIO_PIN_12
#define LCD_CS_GPIO_Port    GPIOB

#define LCD_CD0             LCD_CD_GPIO_Port->BRR = LCD_CD_Pin
#define LCD_CD1             LCD_CD_GPIO_Port->BSRR = LCD_CD_Pin
#define LCD_BKL0            LCD_BKL_GPIO_Port->BRR = LCD_BKL_Pin
#define LCD_BKL1            LCD_BKL_GPIO_Port->BSRR = LCD_BKL_Pin
#define LCD_CS0             LCD_CS_GPIO_Port->BRR = LCD_CS_Pin
#define LCD_CS1             LCD_CS_GPIO_Port->BSRR = LCD_CS_Pin
#define LCD_RST0
#define LCD_RST1

#define LCD_W LCD_GetWidth()
#define LCD_H LCD_GetHeight()

/** 
 * stdout
 * */
#if defined(ENABLE_USB_CDC) || defined(ENABLE_UART)
extern StdOut stdio_ops;
#define SERIAL_IO &stdio_ops
#elif defined(ENABLE_DEBUG)
extern stdout_t dummy_out;
#define SERIAL_IO &dummy_out
#endif


#if defined(ENABLE_USB_CDC)
void MX_USB_DEVICE_Init(void);
#elif defined(ENABLE_UART)

#define UART_TX_PIN     PA_9
#define UART_RX_PIN     PA_10

#define IRQ_PRIORITY_LOW            5 //configLIBRARY_LOWEST_INTERRUPT_PRIORITY

void UART_Init(void);
#endif

/**
 * I2C
 * 
 * PB10 SCL
 * PB11 SDA
 * */
void I2C_Init(void);
//#define I2C_Read(_A, _D, _S) HAL_I2C_Master_Receive(&hi2c2, _A << 1, _D, _S, 100)
//#define I2C_Write(_A, _D, _S) HAL_I2C_Master_Transmit(&hi2c2, _A << 1, _D, _S, 100)
//#define I2C_WriteDMA(_A, _D, _S) i2cSendDMA(_A << 1, _D, _S)
uint16_t I2C_Write(uint8_t addr, uint8_t *data, uint32_t size);
uint16_t I2C_Read(uint8_t addr, uint8_t *dst, uint32_t size);


/**
 * USB
 * 
 * Vile hack to reenumerate, physically _drag_ d+ low.
 * (need at least 2.5us to trigger usb disconnect)
 * */
static inline void reenumerate_usb(void){
    USB->CNTR = USB_CNTR_PDWN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRH = (GPIOA->CRH & ~(0x0F << 16)) | (2 << 16);
    GPIOA->BRR |= GPIO_PIN_12;
    //for (unsigned int i = 0; i < 100000; i++)
	//	__asm__("nop");
}

/**
 * PWM
 * 
 * PB4 PWM1 -> V set
 * PB5 PWM2 -> I set
 * PB0 PWM3 -> I load
 * PB1 PWM4 -> NA
 */
#define PWM_CH_VOLTAGE     (1-1)       // Channel that controls output voltage
#define PWM_CH_CURRENT     (2-1)       // Channel that controls output current
#define PWM_CH_LOAD        (3-1)
#define PWM_RESOLUTION      10UL
#define PWM_MAX_VALUE       (1<<PWM_RESOLUTION)
#define PWM_MIN_VALUE       0
#define PWM_NUM_CH          3          // Number of channels

/**
 * Initialize PWM signal on PB5-4 and PB0 pins
 * Timer 3 is used to generate pwm signals with 10bit resolution,
 * which using a 72MHz system frequency results in a 35,156KHz frequency
 * 
 * \param  pwm1 : PWM1 initial value
 * \param  pwm2 : PWM2 initial value
 * \param  pwm3 : PWM3 initial value
 * */ 
void PWM_Init(uint16_t pwm1, uint16_t pwm2, uint16_t pwm3);

/**
 * Sets duty cycle for the given channel
 * \param  ch     Channel 0-3 -> channel 1-4
 * \param  value  10bit value
 * 
 * \return none
 * */
void PWM_Set(uint8_t, uint16_t);

/**
 * Returns the current value for the given channel
 * \param  ch   
 * \return 16bit pwm value
 * */
uint16_t PWM_Get(uint8_t);


/**
 * ADC
 * */
#ifndef USE_ADCMGR
/**
 * ADC
 *
 * Convertions channel sequence:
 *       1st 2nd 3rd 4th 5th 
 * ADC1:  0   1   2   3   5    V
 * ADC2:  4   4   4   4   6    I
 * */
#define ADC_SEQ_LEN         5
#define ADC_CH_VOLTAGE1     0 // PA6-PA0
#define ADC_CH_VOLTAGE2     1
#define ADC_CH_VOLTAGE3     2
#define ADC_CH_VOLTAGE4     3
#define ADC_CH_CURRENT      4
#define ADC_CH_V_LOAD       5
#define ADC_CH_I_LOAD       6
#else
/**
 * ADC Channel for adc mux
 * */
#define ADC_MUX_CH                  0
#define ADC_PWR_SW_CH               2

#define VOUT_MUX_CH                 0
#define IOUT_MUX_CH                 1
#define VLOAD_MUX_CH                2
#define ILOAD_MUX_CH                3
#define VIN_MUX_CH                  4
#define IUSB_MUX_CH                 13

#define VB4_MUX_CH                   8
#define VB3_MUX_CH                   9
#define VB2_MUX_CH                   10
#define VB1_MUX_CH                   11
#define VB0_MUX_CH                   12

#define TIMED_ADC
#endif

/**
 * @brief Configure ADC in continuos mode using TIM2.
 * Simultaneous convertions are taken and transffered using DMA.
 * Single conversion is performed if adc mux is used
 * 
 * \param ms    Time between convertions
 **/
void ADC_Init(uint16_t);

#ifndef USE_ADCMGR
/**
 *  @brief Configure callback for end of transfer of ADC convertions
 * 
 * \param  cb : call back function for eot
 * \return none
 ************************************************************ */
void ADC_SetCallBack(void (*)(uint16_t*));

/**
 * @brief Get the last performed convertions
 * Not thread safe
 * \param  none
 * \return uint16_t *last_adc_convertions 
 ************************************************************ */
uint16_t *ADC_LastConvertion(void);
#else
/**
 * @brief Configure callback for end of ADC convertion
 * 
 * \param  cb : call back function for eoc
 * \return none
 **/
void ADC_SetCallBack(void (*)(uint16_t));

/**
 * @brief Pause ADC conversions
 * 
 **/
void ADC_Start(void);
void ADC_Stop(void);

/**
 * @brief Perform adc calibration and get resolution based on 
 * internal 1.2V reference
 * 
 * */
void ADC_Calibrate(void);

float ADC_GetResolution(void);
uint16_t ADC_GetCalibration(void);

uint32_t ADC2_Read(uint8_t ch);
#endif

/**
 * RTC
 */  
void RTC_Init(void);

/**
 * Watchdog
 * */
void enableWatchDog(uint32_t interval);
void reloadWatchDog(void);

/**
 * General
 * */
void BOARD_Init(void);

void BOARD_Error_Handler(const char *file, int line);
 
#ifdef __cplusplus
}
#endif

#endif