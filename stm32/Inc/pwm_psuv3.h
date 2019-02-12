#ifndef _pwm_psuv3_h_
#define _pwm_psuv3_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

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

#ifdef __cplusplus
}
#endif
#endif