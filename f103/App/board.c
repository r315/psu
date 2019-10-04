
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