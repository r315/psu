
#include "common.h"


void PWM_Init(uint16_t *initial){

     /* Configure Timer 3 */
    RCC->APB1ENR  |= RCC_APB1ENR_TIM3EN;
    RCC->APB1RSTR |= RCC_APB1ENR_TIM3EN;
    RCC->APB1RSTR &= ~RCC_APB1ENR_TIM3EN;

    TIM3->CR1 = TIM_CR1_ARPE;
    TIM3->CCMR1 = (6<<TIM_CCMR1_OC2M_Pos) | (6<<TIM_CCMR1_OC1M_Pos) | (TIM_CCMR1_OC2PE) | (TIM_CCMR1_OC1PE);  // PWM Mode 1
    TIM3->CCMR2 = (6<<TIM_CCMR2_OC4M_Pos) | (6<<TIM_CCMR2_OC3M_Pos) | (TIM_CCMR2_OC4PE) | (TIM_CCMR2_OC3PE); 
    TIM3->CCER = TIM_CCER_CC4E | TIM_CCER_CC3E | TIM_CCER_CC2E | TIM_CCER_CC1E;
    TIM3->PSC = 1; // Timer3 freq = SystemClock / 2

    TIM3->ARR = (1<<PWM_RESOLUTION) - 1;
    TIM3->CCR1 = initial[0];
    TIM3->CCR2 = initial[1];
    TIM3->CCR3 = initial[2];
    TIM3->CCR4 = initial[3];
    
    TIM3->CR1 |= TIM_CR1_CEN;     // Start pwm before enable outputs

    GPIOA->CRL &= ~(0xFF << 24);
    GPIOA->CRL |= (0xAA << 24);   // PA7-6 AFO-PP
    GPIOB->CRL &= ~(0xFF << 0);
    GPIOB->CRL |= (0xAA << 0);    // PB1-0 AFO-PP
}

/**
 * Set new pwm value for the given channel
 * \param ch        channel to be updated 0-3
 * \param newvalue  New pwm value
 * */
void PWM_Set(uint8_t ch, uint16_t newvalue){

    if(newvalue >  PWM_MAX_VALUE || newvalue < PWM_MIN_VALUE)    
        return;

    uint32_t *ccr = (uint32_t*)&TIM3->CCR1;
    ccr[ch&3] = newvalue;    
}

uint16_t PWM_Get(uint8_t ch){
    uint32_t *ccr = (uint32_t*)&TIM3->CCR1;     
    return ccr[ch&3];
}