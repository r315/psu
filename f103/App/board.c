
#include "board.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "usbd_cdc_if.h"
#include <stdout.h>

void setInterval(void(*cb)(), uint32_t ms){
    // start loop, timer is configures on startup
    // and call is made on interupt handler
    HAL_TIM_Base_Start_IT(&htim4);
}


/**
 * PWM Driver
 * 
 * PWM1 - PA6
 * PWM2 - PA7
 * PWM3 - PB0
 * PWM4 - PB1
 * 
 * \param initial - startup duty for each channel
 * 
 * */
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

/**
 * virtual com port stuff
 * */

#define VC_QUEUE_LENGTH 10
#define VC_QUEUE_ITEM_SIZE 1

static QueueHandle_t chars_queue;

static void vc_init(void){
    chars_queue = xQueueCreate( VC_QUEUE_LENGTH, VC_QUEUE_ITEM_SIZE );
    configASSERT( chars_queue != NULL );
}

// called from cdc interface
void vc_put(uint8_t *c){
    if(chars_queue != NULL)
        xQueueSendFromISR(chars_queue, c, 0);
}

static uint8_t vc_getCharNonBlocking(char *c){
    if(chars_queue == NULL)
        return 0;
    return xQueueReceive(chars_queue, c, 0) == pdPASS;
}

static char vc_getchar(void){
    char c;
    xQueueReceive(chars_queue, &c, portMAX_DELAY);
    return c;
}

uint8_t vc_kbhit(void){
    return VC_QUEUE_LENGTH - uxQueueSpacesAvailable(chars_queue);
}

static void putAndRetry(uint8_t *data, uint16_t len){
uint32_t retries = 1000;
	while(retries--){
		if(	CDC_Transmit_FS(data, len) == USBD_OK)
			break;
	}
}

static void vc_putchar(char c){
	putAndRetry((uint8_t*)&c, 1);
}

static void vc_puts(const char *s){
uint16_t len = 0;
	
	while( *((const char*)(s + len)) != '\0'){
		len++;	
	}
	putAndRetry((uint8_t*)s, len);
}

StdOut vcom = {
    .init = vc_init,
    .xgetchar = vc_getchar,
    .xputchar = vc_putchar,
    .xputs = vc_puts,
    .getCharNonBlocking = vc_getCharNonBlocking,
    .kbhit = vc_kbhit
};

/**
 * I2C DMA driver
 * 
 * This driver was implemented in a way that reduce the most setup steps much as possible,
 * for that the dma is configured once and then all transfers start by the I2C
 * */
void i2cCfgDMA(uint8_t *src, uint16_t size){
    // Configure DMA1 CH4 to handle i2c transmission
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;    
    DMA1_Channel4->CCR =    DMA_CCR_TCIE | // Transfer complete interrupt
                            DMA_CCR_DIR  | // Read From memory
                            DMA_CCR_CIRC |
                            DMA_CCR_MINC;  // Increment memory address
    DMA1_Channel4->CNDTR = size;
    DMA1_Channel4->CPAR = (uint32_t)&I2C2->DR;
    DMA1_Channel4->CMAR = (uint32_t)src;
    DMA1_Channel4->CCR |= DMA_CCR_EN;
    
    // Configure I2C2 transfer
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;    
    I2C2->CCR = 45;           // Freq 400Khz
    I2C2->CR1 = I2C_CR1_PE | I2C_CR1_ACK;
    I2C2->CR2 = 0x24;
    I2C2->CR2 |= I2C_CR2_DMAEN;
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0); // Highest priority
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
} 

/**
 * Due to the implementation of the driver, the parameters
 * data and size are ignored
 * */
void i2cSendDMA(uint8_t address, uint8_t *data, uint16_t size){
uint32_t n = I2C_BUSY_RETRIES;

    
    while(I2C2->SR2 & I2C_SR2_BUSY){ // wait for any transfer to end
        if(--n == 0)
            return;
    }

    I2C2->CR1 |= I2C_CR1_START;     // Send start condition

    n = I2C_BUSY_RETRIES;
    while(!(I2C2->SR1 & I2C_SR1_SB)){ // wait for master mode
        if(--n == 0)
            return;
    }    
    
    I2C2->CR2 |= I2C_CR2_DMAEN;         // enable DMA
    I2C2->DR = address;                 
    while(!(I2C2->SR1 & I2C_SR1_ADDR)){ // wait for slave acknowledge       
        if(--n == 0)            
            return;

        if(I2C2->SR1 & I2C_SR1_AF){
            I2C2->SR1 &= ~(I2C_SR1_AF);
            I2C2->CR1 |= I2C_CR1_STOP;  
            return;      
        }
    }
    n = I2C2->SR2; // Dummy read for crearing flags
    //while(I2C2->SR2 & I2C_SR2_BUSY);   
}

/**
 * ADC Driver
 * */

#define ADC_TRIGGER_CLOCK 500000UL
#define ADC_CR1_DUALMOD_FAST_INTERLEAVED     (7 << 16)
#define ADC_CR1_DUALMOD_SIMULTANEOUS         (1 << 16)
#define ADC_SAMPLES 4
#define ADC_SQR1_L_(len)                     (len << 20)
#define ADC_SQR3_SQ1_(ch)                    (ch << 0)
#define ADC_SQR3_SQ2_(ch)                    (ch << 5)
#define ADC_SQR3_SQ3_(ch)                    (ch << 10)
#define ADC_SQR3_SQ4_(ch)                    (ch << 15)
#define ADC_SMPR2_SMP0_(cy)                    (cy << 0)
#define ADC_SMPR2_SMP1_(cy)                    (cy << 3)
#define ADC_SMPR2_SMP2_(cy)                    (cy << 6)
#define ADC_SMPR2_SMP3_(cy)                    (cy << 9)

static void (*eotcb)(uint16_t*);
static uint16_t adcres[ADC_SAMPLES];

/* ***********************************************************
 * ADC is triggered by TIM2 TRGO and performs dual 
 * simultaneous convertion on regular simultaneous mode. 
 * It performs the conversion of 4 channels and transfers 
 * the result to memory using DMA
 * At the end of convertion, optionaly a callback function 
 * may be invoked
 * 
 * \param ms    Time between convertions
 ************************************************************ */

void ADC_Init(uint16_t ms){

    /* Configure DMA Channel1*/
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;           // Enable DMA1

    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;  // Source address ADC1 Data Register
    DMA1_Channel1->CMAR = (uint32_t)adcres;     // Destination address memory
    DMA1_Channel1->CNDTR =  ADC_SAMPLES/2;      // We are going to transfer 32-bit
    DMA1_Channel1->CCR =    DMA_CCR_PL |        // Highest priority
                            DMA_CCR_MSIZE_1 |   // 32bit Dst size
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

    ADC1->CR1 = ADC_CR1_DUALMOD_SIMULTANEOUS |
                ADC_CR1_SCAN;               // Scan throu all channels on sequence

    ADC1->SQR1 = ADC_SQR1_L_(1);            // Two channels on sequence
    ADC1->SQR3 = ADC_SQR3_SQ1_(0) |         // First convertion CH0, second CH2 
                 ADC_SQR3_SQ2_(2);

    ADC1->SMPR2 = ADC_SMPR2_SMP0_(7) |      // CH0 and CH2 sample time, 239.5 cycles
                  ADC_SMPR2_SMP2_(7);
                
    /* Configure ADC 2 */
    RCC->APB2ENR  |= RCC_APB2ENR_ADC2EN;    // Enable Adc2
    RCC->APB2RSTR |= RCC_APB2ENR_ADC2EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_ADC2EN;

    ADC2->CR2 = ADC_CR2_EXTTRIG  |
    		    ADC_CR2_EXTSEL_2 |          // Select SWSTART for dual convertion
				ADC_CR2_EXTSEL_1 |
    		    ADC_CR2_EXTSEL_0 |
				ADC_CR2_ADON;

    ADC2->SQR1 = ADC_SQR1_L_(1);            // Two channels on sequence
    ADC2->SQR3 = ADC_SQR3_SQ1_(1) |         // first convertion CH1, second CH3 
                 ADC_SQR3_SQ2_(3);

    ADC2->SMPR2 = ADC_SMPR2_SMP1_(7) |      // CH1 and CH3 sample time, 239.5 cycles
                  ADC_SMPR2_SMP3_(7);

    ADC2->CR1 = ADC_CR1_SCAN;

    /* Configure GPIOA Pins */
    GPIOA->CRL &= ~0xFFFF;                

    NVIC_SetPriority(DMA1_Channel1_IRQn, 0); // Highest priority
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    eotcb = NULL;
    TIM2->CR1 |= TIM_CR1_CEN;
}


uint16_t *ADC_LastConvertion(void){
    return adcres;
}

/**
 * DMA transfer interrupt handler
 * */
void DMA1_Channel1_IRQHandler(void){

    if(DMA1->ISR & DMA_ISR_TCIF1){
        ADC1->SR = 0;                    // Clear ADC1 Flags
        ADC2->SR = 0;
            if(eotcb != NULL)
        eotcb(adcres);
    }

    DMA1->IFCR = DMA_IFCR_CGIF1;    // Clear DMA Flags TODO: ADD DMA Error handling ?
}


void ADC_SetCallBack(void (*cb)(uint16_t*)){
    eotcb = cb;
}
