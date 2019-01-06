
#include "common.h"

void PWM_Init(uint32_t freq, uint16_t value){

     /* Configure Timer 3 */
    RCC->APB1ENR  |= RCC_APB1ENR_TIM3EN;
    RCC->APB1RSTR |= RCC_APB1ENR_TIM3EN;
    RCC->APB1RSTR &= ~RCC_APB1ENR_TIM3EN;


    
    TIM3->CCMR1 = (6<<TIM_CCMR1_OC2M_Pos) | (6<<TIM_CCMR1_OC1M_Pos);  // PWM Mode 1
    TIM3->CCMR2 = (6<<TIM_CCMR2_OC4M_Pos) | (6<<TIM_CCMR2_OC3M_Pos); 
    TIM3->CCER = TIM_CCER_CC4E | TIM_CCER_CC3E | TIM_CCER_CC2E | TIM_CCER_CC1E;
    TIM3->PSC = (SystemCoreClock/(freq*100)) - 1;

    TIM3->ARR = 1024;
    TIM3->CCR1 = value;
    TIM3->CCR2 = value;
    TIM3->CCR3 = value;
    TIM3->CCR4 = value;

    GPIOA->CRL &= ~(0xFF << 24);
    GPIOA->CRL |= (0x22 << 24);   // PA7-6 O-PP
    GPIOB->CRL &= ~(0xFF << 0);
    GPIOB->CRL |= (0x22 << 0);   // PB1-0 O-PP

    TIM3->CR1 |= TIM_CR1_CEN;

}