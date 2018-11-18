#ifndef _board_h_
#define _board_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

/**
 * HW symbols for button handling
 * */
#define BUTTON_LEFT  	(1<<15)
#define BUTTON_RIGHT 	(1<<13)
#define BUTTON_CENTER	(1<<14)
#define BUTTON_A 		  BUTTON_CENTER

#define BUTTON_Capture() (~GPIOB->IDR & BUTTON_MASK)
#define BUTTON_MASK (BUTTON_LEFT | BUTTON_RIGHT | BUTTON_A)

#define GetTicks HAL_GetTick

inline uint32_t ElapsedTicks(uint32_t start_ticks){ 
    uint32_t current = GetTicks(); 
    return (current > start_ticks) ? current - start_ticks : 0xFFFFFFFF - start_ticks + current;
}

#ifdef __cplusplus
}
#endif

#endif