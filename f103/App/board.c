
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
 * I2C striped down DMA
 * */
void i2cCfgDMA(uint8_t *src, uint16_t size){
    // Configure DMA1 CH4 to handle i2c transmission
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel4->CNDTR = size;
    DMA1_Channel4->CPAR = (uint32_t)&I2C2->DR;
    DMA1_Channel4->CMAR = (uint32_t)src;
    DMA1_Channel4->CCR =    DMA_CCR_EN   | 
                            DMA_CCR_TCIE | // Transfer complete interrupt
                            DMA_CCR_DIR  | // Read From memory
                            DMA_CCR_CIRC |
                            DMA_CCR_MINC;  // Increment memory address
    
    
    // Configure I2C2 transfer
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;    
    I2C2->CCR = 45;           // Freq 400Khz
    I2C2->CR1 = I2C_CR1_PE | I2C_CR1_ACK;
    I2C2->CR2 = 0x24;
    I2C2->CR2 |= I2C_CR2_DMAEN;
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0); // Highest priority
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
} 


void i2cSendDMA(uint8_t address, uint8_t *data, uint16_t size){
uint32_t n;

    while(I2C2->SR2 & I2C_SR2_BUSY);    

    I2C2->CR1 |= I2C_CR1_START;

    n = 1000;
    while(!(I2C2->SR1 & I2C_SR1_SB)){
        if(--n == 0)
            return;
    }    
    
    I2C2->CR2 |= I2C_CR2_DMAEN;
    I2C2->DR = address;
    while(!(I2C2->SR1 & I2C_SR1_ADDR)){        
        if(--n == 0)            
            return;

        if(I2C2->SR1 & I2C_SR1_AF){
            I2C2->SR1 &= ~(I2C_SR1_AF);
            I2C2->CR1 |= I2C_CR1_STOP;  
            return;      
        }
    }
    n = I2C2->SR2; // Dummy read
    while(I2C2->SR2 & I2C_SR2_BUSY);   
}
