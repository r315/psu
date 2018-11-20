#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "common.h"
#include "tim.h"

#define ADC_TRIGGER_CLOCK 500000UL
#define ADC_CR1_DUALMOD_FAST_INTERLEAVED     (7 << 16)
#define ADC_CR1_DUALMOD_SIMULTANEOUS         (1 << 16)
#define ADC_SAMPLES 4


Console console;

class VoltageDro : public Command{
    double v = 0;
public:
    char execute(void *ptr){
        SEVEN_Double(1,1,v);
        v += 0.5;
        return CMD_OK;
    }
    void help(void){}

    VoltageDro(void) : Command("vdro") {}
};

enum Mode{
    PSU,
    LOAD
};

enum State{
    RUN
};

class Instrument{
    uint32_t ticks;
public:
    double vout;
    Mode mode;
    State state;

    uint32_t isTimed(void){
        if(ticks == 0)
            return YES;

        return !(--ticks);
    } 

};

static Instrument psu;
volatile int done = 0;
uint16_t adcres[4];

/**
 * DMA transfer end (all data) interrupt handler
 * */
extern "C" void DMA1_Channel1_IRQHandler(void){
   
    DMA1->IFCR |= DMA_IFCR_CGIF1;    // Clear DMA Flags TODO: ADD DMA Error handling ?
    DMA1_Channel1->CNDTR = ADC_SAMPLES;
    ADC1->SR = 0;                    // Clear ADC1 Flags
    SEVEN_Double(1,1, (3.3 * adcres[0]) / 4096);
    SEVEN_Double(65,1, (3.3 * adcres[1]) / 4096);
    LCD_Update();
    done += 1;
}

/* ***********************************************************
 * ADC is triggered by TIM3 TRGO and performs dual 
 * simultaneous convertion. it converts 4 channels and transfers 
 * the result to memory using DMA 
 * 
 * \param ms    Time between events
 * \param dst   pointer to destination, at least 4*uint16_t
 ************************************************************ */

void ADC_Init(uint16_t ms, uint16_t *dst){

    /* Configure DMA Channel1*/
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;           // Enable DMA1

    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;  // Source address ADC1 Data Register
    DMA1_Channel1->CMAR = (uint32_t)dst;        // Destination address memory
    DMA1_Channel1->CNDTR = ADC_SAMPLES;
    DMA1_Channel1->CCR =    DMA_CCR_PL |        // Highest priority
                            DMA_CCR_MSIZE_0 |   // 16bit Dst size
                            DMA_CCR_PSIZE_1 |   // 32bit src size
                            DMA_CCR_MINC |      // increment memory pointer after transference
                            DMA_CCR_CIRC |      // Circular mode
                            DMA_CCR_TCIE;       // Enable end of transfer interrupt    
    DMA1_Channel1->CCR |=   DMA_CCR_EN;

     /* Configure Timer 3 */
    RCC->APB1ENR  |= RCC_APB1ENR_TIM3EN;    // Enable Timer 3
    RCC->APB1RSTR |= RCC_APB1ENR_TIM3EN;    // Reset timer registers
    RCC->APB1RSTR &= ~RCC_APB1ENR_TIM3EN;

    TIM3->CR2 = (4<<TIM_CR2_MMS_Pos);       // OC1REF signal is used as trigger output (TRGO)
    TIM3->CCMR1 = (3<<TIM_CCMR1_OC1M_Pos);  // Toggle OC1REF on match
    TIM3->CCER = TIM_CCER_CC1E;             // Enable compare output for CCR1
    TIM3->PSC = (SystemCoreClock/ADC_TRIGGER_CLOCK) - 1; // 500kHz clock

    TIM3->ARR = (ms * (SystemCoreClock/ADC_TRIGGER_CLOCK)) - 1;
    TIM3->CCR1 = TIM3->ARR;

#if !defined (OUTPUT_ADC_TRIGGER)
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
            ADC_CR2_EXTTRIG |               // Only the rising edge of external signal can start the conversion
            ADC_CR2_EXTSEL_2 |              // Select TIM3_TRGO as Trigger source
            ADC_CR2_DMA;                    // Enable DMA Request
    ADC1->CR1 = ADC_CR1_DUALMOD_SIMULTANEOUS;

    //ADC1->SQR1 = (1 << 20);                 // One convertion
    //ADC1->SQR3 = (2 << 5) | (0 << 0);       // First convertion CH0, second CH2

    /* Configure ADC 2 */
    RCC->APB2ENR  |= RCC_APB2ENR_ADC2EN;    // Enable Adc2
    RCC->APB2RSTR |= RCC_APB2ENR_ADC2EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_ADC2EN;

    ADC2->CR2 = ADC_CR2_EXTTRIG  |
    		    ADC_CR2_EXTSEL_2 |          // Select SWSTART for dual convertion
				ADC_CR2_EXTSEL_1 |
    		    ADC_CR2_EXTSEL_0 |
				ADC_CR2_ADON;
    //ADC2->CR1 |= (1 << 11);

    //ADC2->SQR1 = (1 << 20);                // Two convertions.        
    //ADC2->SQR3 = (3 << 5) | (1 << 0);      // first convertion CH1, second CH3 

                   
                 

    NVIC_SetPriority(DMA1_Channel1_IRQn, 0); // Highest priority
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

     TIM3->CR1 |= TIM_CR1_CEN;
}


void ADC_Done(uint16_t value){

}


void handleButtons(void){
    BUTTON_Read();
    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        switch(BUTTON_GetValue()){
            case BUTTON_LEFT:
                psu.vout += 0.01;
                break;

            case BUTTON_RIGHT: 
                psu.vout -= 0.01;
                break;
        }
        SEVEN_Double(1,1,psu.vout);
        LCD_Update();
    }
}

/**
 * Called every 10ms
 * */
extern "C" void psu_v3_loop(void){
    
    handleButtons();
     
    /*    switch(psu.mode){
            case PSU: break;
            case LOAD: break;
        }
    LCD_Update();*/

    HAL_GPIO_TogglePin(GPIOA, DBG_Pin);
}

extern "C" void psu_v3(void){
VoltageDro vdro;
CmdAdc adc1;


    HAL_TIM_Base_Start_IT(&htim4);

    ADC_Init(100,adcres);

    console.addCommand(&vdro);
    console.addCommand(&adc1);

    while(1){
        console.process();

       
    }
}


