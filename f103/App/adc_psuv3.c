
#include "board.h"


static void (*eotcb)(uint16_t*);
static uint16_t adcres[ADC_SAMPLES];

/* ***********************************************************
 * ADC is triggered by TIM3 TRGO and performs dual 
 * simultaneous convertion. it converts 4 channels and transfers 
 * the result to memory using DMA 
 * 
 * \param ms    Time between convertions
 ************************************************************ */

void ADC_Init(uint16_t ms){

    /* Configure DMA Channel1*/
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;           // Enable DMA1

    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;  // Source address ADC1 Data Register
    DMA1_Channel1->CMAR = (uint32_t)adcres;        // Destination address memory
    DMA1_Channel1->CNDTR = ADC_SAMPLES;
    DMA1_Channel1->CCR =    DMA_CCR_PL |        // Highest priority
                            DMA_CCR_MSIZE_0 |   // 16bit Dst size
                            DMA_CCR_PSIZE_1 |   // 32bit src size
                            DMA_CCR_MINC |      // increment memory pointer after transference
                            DMA_CCR_CIRC |      // Circular mode
                            DMA_CCR_TCIE;       // Enable end of transfer interrupt    
    DMA1_Channel1->CCR |=   DMA_CCR_EN;

     /* Configure Timer 2 */
    RCC->APB1ENR  |= RCC_APB1ENR_TIM2EN;    // Enable Timer 2
    RCC->APB1RSTR |= RCC_APB1ENR_TIM2EN;    // Reset timer registers
    RCC->APB1RSTR &= ~RCC_APB1ENR_TIM2EN;

    TIM2->CCMR1 = (3<<TIM_CCMR1_OC2M_Pos);  // Toggle OC2REF on match
    TIM2->CCER = TIM_CCER_CC2E;             // Enable compare output for CCR2
    TIM2->PSC = (SystemCoreClock/ADC_TRIGGER_CLOCK) - 1; // 500kHz clock

    TIM2->ARR = (ms * (SystemCoreClock/ADC_TRIGGER_CLOCK)) - 1;
    TIM2->CCR2 = TIM2->ARR;

#if defined (OUTPUT_ADC_TRIGGER)
    /* Configure TIM3_CH1 Output PA6 */
    GPIOA->CRL &= ~(0x0F << 24); // Clear mode and configuration
    GPIOA->CRL |= (0x0A << 24);  // Output 2MHz, Alternative function push-pull
#endif

    /* Configure ADC 1 */
    RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN;     // Enable Adc1
    RCC->APB2RSTR |= RCC_APB2ENR_ADC1EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_ADC1EN;

    ADC1->CR2 = ADC_CR2_ADON;               // Turn on ADC1
    ADC1->CR2 |=
            ADC_CR2_EXTTRIG  |              // Only the rising edge of external signal can start the conversion
            //ADC_CR2_EXTSEL_2 |              // 0b100 Select TIM3_TRGO as Trigger source
            ADC_CR2_EXTSEL_1 |              // 0b011 Select TIM2_CC2 Event
            ADC_CR2_EXTSEL_0 |              // 
            ADC_CR2_DMA;                    // Enable DMA Request
    ADC1->CR1 = ADC_CR1_DUALMOD_SIMULTANEOUS;

    ADC1->SQR1 = (1 << 20);                 // Two convertions
    ADC1->SQR3 = (2 << 5) | (0 << 0);       // First convertion CH0, second CH2
    ADC1->CR1 |= ADC_CR1_DISCEN |           // Convert ONE channel in discontinous mode 
                (0 << ADC_CR1_DISCNUM_Pos);
                

    /* Configure ADC 2 */
    RCC->APB2ENR  |= RCC_APB2ENR_ADC2EN;    // Enable Adc2
    RCC->APB2RSTR |= RCC_APB2ENR_ADC2EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_ADC2EN;

    ADC2->CR2 = ADC_CR2_EXTTRIG  |
    		    ADC_CR2_EXTSEL_2 |          // Select SWSTART for dual convertion
				ADC_CR2_EXTSEL_1 |
    		    ADC_CR2_EXTSEL_0 |
				ADC_CR2_ADON;

    ADC2->SQR1 = (1 << 20);                //        
    ADC2->SQR3 = (3 << 5) | (1 << 0);      // first convertion CH1, second CH3 
    ADC2->CR1 |= (0 << ADC_CR1_DISCNUM_Pos) | ADC_CR1_DISCEN;                 

    NVIC_SetPriority(DMA1_Channel1_IRQn, 0); // Highest priority
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;

    eotcb = NULL;
}


uint16_t *ADC_LastConvertion(void){
    return adcres;
}

/**
 * DMA transfer end (all data) interrupt handler
 * */
void DMA1_Channel1_IRQHandler(void){ 
    DMA1->IFCR |= DMA_IFCR_CGIF1;    // Clear DMA Flags TODO: ADD DMA Error handling ?
    DMA1_Channel1->CNDTR = ADC_SAMPLES;
    ADC1->SR = 0;                    // Clear ADC1 Flags
    if(eotcb != NULL)
        eotcb(adcres);   
}


void ADC_SetCallBack(void (*cb)(uint16_t*)){
    eotcb = cb;
}