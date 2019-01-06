#ifndef _pwm_psuv3_h_
#define _pwm_psuv3_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define PWM_FREQ 10000UL

/**
 * Initialyse pwm on PA7-6, PB1-0 
 * 
 * \param  freq     PWM signal frequency
 * \param  value    initial value [0...]
 * */ 
void PWM_Init(uint32_t freq, uint16_t value);

#ifdef __cplusplus
}
#endif
#endif