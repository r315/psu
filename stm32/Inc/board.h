#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"

/**
 * HW symbols for button handling
 * */
#define BUTTON_LEFT  	(1<<15)
#define BUTTON_RIGHT 	(1<<13)
#define BUTTON_CENTER	(1<<14)
#define BUTTON_A 		  BUTTON_CENTER

#define BUTTON_Capture() (~GPIOB->IDR & BUTTON_MASK)
#define BUTTON_MASK (BUTTON_LEFT | BUTTON_RIGHT | BUTTON_A)
#define BUTTON_Cfg()

#define GetTicks HAL_GetTick

inline uint32_t ElapsedTicks(uint32_t start_ticks){ 
    uint32_t current = GetTicks(); 
    return (current > start_ticks) ? current - start_ticks : 0xFFFFFFFF - start_ticks + current;
}

#define DelayMs(d) HAL_Delay(d)


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