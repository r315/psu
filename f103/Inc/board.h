#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <button.h>
#include <stdout.h>
//#include "stm32f1xx_hal.h"
#include "main.h"
#include "stm32f1xx.h"
#include "ssd1306.h"
#include "adc_psuv3.h"

#define USE_I2C_DMA
#define I2C_Write(_A, _D, _S) HAL_I2C_Master_Transmit(&hi2c2, _A << 1, _D, _S, 100)
#define I2C_WriteDMA(_A, _D, _S) i2cSendDMA(_A << 1, _D, _S)


/**
 * HW symbols for button handling
 * */
#define BUTTON_UP       (1<<0)
#define BUTTON_DOWN     (1<<1)
#define BUTTON_LEFT  	(1<<2)
#define BUTTON_RIGHT 	(1<<3)
#define BUTTON_SET  	(1<<4)
#define BUTTON_MODE     (1<<5)
#define BUTTON_OUT      (1<<6)
#define BUTTON_MEM      (1<<7)

#define BUTTON_HW_INIT
#define BUTTON_HW_READ (~GPIOB->IDR & BUTTON_MASK)
#define BUTTON_MASK (BUTTON_LEFT | BUTTON_RIGHT | BUTTON_UP   | BUTTON_DOWN |\
                     BUTTON_SET  | BUTTON_OUT   | BUTTON_MODE | BUTTON_MEM)

#define SPOWER_OFF \
    GPIOA->CRL = (GPIOA->CRH & ~(0x0F << 8)) | (2 << 8); \
    GPIOA->ODR &= ~GPIO_PIN_2; \
    while(1);

#define GetTicks HAL_GetTick
#define DelayMs(d) HAL_Delay(d)

static inline uint32_t ElapsedTicks(uint32_t start_ticks){ 
    uint32_t current = GetTicks(); 
    return (current > start_ticks) ? current - start_ticks : 0xFFFFFFFF - start_ticks + current;
}

/** 
 * Global variables
 * */
extern I2C_HandleTypeDef hi2c2;
extern TIM_HandleTypeDef htim4;
extern StdOut vcom;

/**
 * Function prototypes
 * */
void setInterval(void(*cb)(), uint32_t ms);
void i2cCfgDMA(uint8_t *src, uint16_t size);
void i2cSendDMA(uint8_t address, uint8_t *data, uint16_t size);
/*
* Vile hack to reenumerate, physically _drag_ d+ low.
* (need at least 2.5us to trigger usb disconnect)
*/
static inline void reenumerate_usb(void){
    USB->CNTR = USB_CNTR_PDWN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRH = (GPIOA->CRH & ~(0x0F << 16)) | (2 << 16);
    GPIOA->BRR |= GPIO_PIN_12;
    //for (unsigned int i = 0; i < 100000; i++)
	//	__asm__("nop");
}

#define LCD_W SSD1306_LCDWIDTH
#define LCD_H SSD1306_LCDHEIGHT

/**
 * PWM
 */
#define PWM_RESOLUTION 10UL
#define PWM_MAX_VALUE (1<<PWM_RESOLUTION)
#define PWM_MIN_VALUE 5

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
 * \retusn 16bit pwm value
 * */
uint16_t PWM_Get(uint8_t);

/**
 * Analog Pins
 * */
 
 // VOUT PA0
 // IOUT PA1
 // VLOAD PA2
 // ILOAD PA3
 
 /**
 * PWM Pins
 * */
 // PB0 PWM_VOUT
 // PB1 PWM_IOUT
 // PWM_ILOAD
 // PWM_VLOAD
 
 /**
 * I2C
 * */
 // PB10 SCL
 // PB11 SDA
 
 
 
 
 
#ifdef __cplusplus
}
#endif

#endif