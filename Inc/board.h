#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <button.h>
#include <stdout.h>
#include "main.h"
#include "stm32f1xx.h"
#include "pcf8574.h"
#include "lcd.h"
#include "st7735.h"
#include "adcmgr.h"
#include "pinName.h"


/**
 * Button 
 * */
#define BUTTON_UP       (1<<1)
#define BUTTON_DOWN     (1<<0)
#define BUTTON_LEFT  	(1<<3)
#define BUTTON_RIGHT 	(1<<2)
#define BUTTON_SET  	(1<<4)
#define BUTTON_MODE     (1<<5)
#define BUTTON_OUT      (1<<6)
#define BUTTON_MEM      (1<<7)

#define BUTTON_HW_INIT
#define BUTTON_HW_READ ~EXPANDER_Read()
#define BUTTON_MASK (BUTTON_LEFT | BUTTON_RIGHT | BUTTON_UP   | BUTTON_DOWN |\
                     BUTTON_SET  | BUTTON_OUT   | BUTTON_MODE | BUTTON_MEM)
#define BUTTON_VALUE (uint8_t)BUTTON_GetValue()

// TODO: Properly disable all peripherals (clock pwr) and then power off
#define SPOWER_OFF \
    GPIOA->CRL = (GPIOA->CRH & ~(0x0F << 8)) | (2 << 8); \
    GPIOA->ODR &= ~GPIO_PIN_2; \
    while(1);

/**
 * LEDS and debug pin
 * */
#if 1
#define LED_PIN     PC_13
#define LED_INIT    pinInit(LED_PIN, GPO_2MHZ);
#else// GPIO
#define LED_INIT
#endif

#define LED_TOGGLE HAL_GPIO_TogglePin(PIN_NAME_TO_PORT(LED_PIN), PIN_NAME_TO_PIN(LED_PIN))
#define LED_OFF pinWrite(LED_PIN, GPIO_PIN_SET)
#define LED_ON  pinWrite(LED_PIN, GPIO_PIN_RESET)
#define DBG_PIN_HIGH LED_OFF
#define DBG_PIN_LOW LED_ON
#define DBG_PIN_TOGGLE LED_TOGGLE

/**
 * Delay and tick count
 * */
#define GetTicks HAL_GetTick
#define DelayMs(d) HAL_Delay(d)

static inline uint32_t ElapsedTicks(uint32_t start_ticks){ 
    uint32_t current = GetTicks(); 
    return (current > start_ticks) ? current - start_ticks : 0xFFFFFFFF - start_ticks + current;
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
 * */
#define TFT_W 80
#define TFT_H 160  // 162 on GRAM

#define TFT_OFFSET_SOURCE	26
#define TFT_OFFSET_GATE		1
#define TFT_BGR_FILTER

#define LCD_CD_Pin          GPIO_PIN_14
#define LCD_CD_GPIO_Port    GPIOB
#define LCD_BKL_Pin         GPIO_PIN_4
#define LCD_BKL_GPIO_Port   GPIOB
#define LCD_RST_Pin         GPIO_PIN_3
#define LCD_RST_GPIO_Port   GPIOB
#define LCD_CS_Pin          GPIO_PIN_12
#define LCD_CS_GPIO_Port    GPIOB

#define LCD_CD0             LCD_CD_GPIO_Port->BRR = LCD_CD_Pin
#define LCD_CD1             LCD_CD_GPIO_Port->BSRR = LCD_CD_Pin
#define LCD_BKL0            LCD_BKL_GPIO_Port->BRR = LCD_BKL_Pin
#define LCD_BKL1            LCD_BKL_GPIO_Port->BSRR = LCD_BKL_Pin
#define LCD_CS0             LCD_CS_GPIO_Port->BRR = LCD_CS_Pin
#define LCD_CS1             LCD_CS_GPIO_Port->BSRR = LCD_CS_Pin
#define LCD_RST0            LCD_RST_GPIO_Port->BRR = LCD_RST_Pin
#define LCD_RST1            LCD_RST_GPIO_Port->BSRR = LCD_RST_Pin

//LCD_CD_GPIO_Port->CRH = (LCD_CD_GPIO_Port->CRH & ~(0xF << 4)) | (2 << 4);
//GPIOB->CRL = (GPIOA->CRL & ~(0xF << (4<<2))) | (2 <<(4<<2));
//GPIOB->CRL = (GPIOA->CRL & ~(0xF << (3<<2))) | (2 <<(3<<2));

#define LCD_W LCD_GetWidth()
#define LCD_H LCD_GetHeight()

/**
 *  NVDATA 
 * */
#define NVDATA_SECTOR_INIT      
#define NVDATA_SECTOR_START     &_seeprom
#define NVDATA_SECTOR_END       &_eeeprom
#define NVDATA_SECTOR_READ      memcpy
#define NVDATA_SECTOR_WRITE     flashWrite
#define NVDATA_SECTOR_ERASE     flashPageErase
#define EEPROM_Read             NV_Read
#define EEPROM_Write(_A,_B,_C)  NV_Write(_A,_B,_C)
#define EEPROM_Sync             NV_Sync
#define EEPROM_SIZE             30
extern uint32_t _seeprom, _eeeprom;     //declared on linker script
uint32_t flashWrite(uint8_t *dst, uint8_t *data, uint16_t count);
uint32_t flashPageErase(uint32_t PageAddress);
void FLASH_PageErase(uint32_t PageAddress);       // HAL Function

/** 
 * stdout
 * */
#if defined(ENABLE_USB_CDC) || defined(ENABLE_UART)
extern StdOut stdio_ops;
#define DEFSTDIO stdio_ops
#elif defined(ENABLE_DEBUG)
extern stdout_t dummy_out;
#define DEFSTDIO dummy_out
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
void I2C_Write(uint8_t addr, uint8_t *data, uint32_t size);
void I2C_Read(uint8_t addr, uint8_t *dst, uint32_t size);


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
#define PWM_NUM_CH          4          // Number of channels

/**
 * Initialyse PWM signal on PA7-6 and PB1-0 pins
 * Timer 3 is used to generate pwm signals with 10bit resolution,
 * which using a 72MHz system frequency results in a 35,156KHz frequency
 * 
 * \param  none
 * */ 
void PWM_Init(uint16_t *);

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
#define ADCMUX_CHANNEL      0

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

/**
 * @brief Perform adc calibration and get resolution based on 
 * internal 1.2V reference
 * 
 * */
void ADC_Calibrate(void);

float ADC_GetResolution(void);
uint16_t ADC_GetCalibration(void);
#endif

/**
 * RTC
 */  
void RTC_Init(void);

/**
 * General
 * */
void BOARD_Init(void);

void Error_Handler(const char *file, int line);
 
#ifdef __cplusplus
}
#endif

#endif