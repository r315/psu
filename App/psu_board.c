
#include "board.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "usbd_cdc_if.h"
#include <stdout.h>

static I2C_HandleTypeDef hi2c2;


void BOARD_Init(void){
    
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;
    
    LED_INIT;
    SPI_Init();
    I2C_Init();
    RTC_Init();

    LCD_Init();
    LCD_Rotation(LCD_REVERSE_LANDSCAPE);
    EXPANDER_Init();
}

/**
 * @brief Time base configuration.
 * SysTick is assigned to FreeRTOS, 
 * Timer 4 is used to generate a 1ms time base instead
 * */
void TICK_Init(void){

    RCC->APB1ENR  |= RCC_APB1ENR_TIM4EN;
    RCC->APB1RSTR |= RCC_APB1ENR_TIM4EN;
    RCC->APB1RSTR &= ~RCC_APB1ENR_TIM4EN;

    TIM4->PSC = (uint32_t)((SystemCoreClock / 1000000UL) - 1);
    TIM4->ARR = 1000 - 1;
    TIM4->DIER |= TIM_DIER_UIE;

    HAL_NVIC_SetPriority(TIM4_IRQn, 0 ,0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
    TIM4->CR1 = TIM_CR1_CEN;
}

void TIM4_IRQHandler(void){
    TIM4->SR = ~TIM_IT_UPDATE;
    HAL_IncTick();
}

/**
 * PWM Driver
 * 
 * PWM1 - PB4
 * PWM2 - PB5
 * PWM3 - PB0
 * PWM4 - PB1
 * 
 * \param initial - startup duty for each channel
 * 
 * */
void PWM_Init(void){

     /* Configure Timer 3 */
    RCC->APB1ENR  |= RCC_APB1ENR_TIM3EN;
    RCC->APB1RSTR |= RCC_APB1ENR_TIM3EN;
    RCC->APB1RSTR &= ~RCC_APB1ENR_TIM3EN;

    TIM3->CR1 = TIM_CR1_ARPE;
    TIM3->CCMR1 = (6<<TIM_CCMR1_OC2M_Pos) | (6<<TIM_CCMR1_OC1M_Pos) | (TIM_CCMR1_OC2PE) | (TIM_CCMR1_OC1PE);  // PWM Mode 1
    TIM3->CCMR2 = (6<<TIM_CCMR2_OC4M_Pos) | (6<<TIM_CCMR2_OC3M_Pos) | (TIM_CCMR2_OC4PE) | (TIM_CCMR2_OC3PE); 
    TIM3->CCER = TIM_CCER_CC4E | TIM_CCER_CC3E | TIM_CCER_CC2E | TIM_CCER_CC1E;
    TIM3->PSC = 1; // Timer3 freq = SystemClock / 2

    TIM3->ARR = PWM_MAX_VALUE - 1;
    TIM3->CCR1 = (PWM_MAX_VALUE >> 1) - 1;
    TIM3->CCR2 = (PWM_MAX_VALUE >> 1) - 1;
    TIM3->CCR3 = (PWM_MAX_VALUE >> 1) - 1;
    TIM3->CCR4 = (PWM_MAX_VALUE >> 1) - 1;
    
    TIM3->CR1 |= TIM_CR1_CEN;     // Start pwm before enable outputs

    /* Configure PWM pins */
    GPIOB->CRL &= ~(0xFF00FF << 0); // PB5-PB4, PB1-PB0
    GPIOB->CRL |= (0xAA00AA << 0);
    // remap PB5-PB5
    AFIO->MAPR |= (2 << 10);    
}

/**
 * Set new pwm value for the given channel
 * \param ch        channel to be updated 0-3
 * \param newvalue  New pwm value
 * */
void PWM_Set(uint8_t ch, uint16_t newvalue){

    if(newvalue >  PWM_MAX_VALUE || newvalue < PWM_MIN_VALUE)    
        return;

    ((uint32_t*)&TIM3->CCR1)[ch & 3] = newvalue;
}

uint16_t PWM_Get(uint8_t ch){
    uint32_t *ccr = (uint32_t*)&TIM3->CCR1;     
    return ccr[ch&3];
}

/**
 * Console stdout, stdin
 * */
#define STDIN_QUEUE_LENGTH 128
#define STDOUT_QUEUE_LENGTH 128
#define STDIO_QUEUE_ITEM_SIZE 1

static QueueHandle_t stdin_queue;
static QueueHandle_t stdout_queue;

// called from ISR
void stdin_queue_char(uint8_t *c){
    xQueueSendToBackFromISR(stdin_queue, c, NULL);
}

static uint8_t stdin_try_dequeue(char *c){
    if(stdin_queue == NULL)
        return 0;
    return xQueueReceive(stdin_queue, c, 0) == pdPASS;
}

static char stdin_wait_char(void){
    char c;
    xQueueReceive(stdin_queue, &c, portMAX_DELAY);
    return c;
}

uint8_t stdin_queued(void){
    return STDIN_QUEUE_LENGTH - uxQueueSpacesAvailable(stdin_queue);
}

static void stdio_init(void){
    stdin_queue = xQueueCreate( STDIN_QUEUE_LENGTH, STDIO_QUEUE_ITEM_SIZE );
    configASSERT( stdin_queue != NULL );

    stdout_queue = xQueueCreate( STDIN_QUEUE_LENGTH, STDIO_QUEUE_ITEM_SIZE );
    configASSERT( stdout_queue != NULL );

    #if defined(ENABLE_USB_CDC)
    MX_USB_DEVICE_Init();
    #elif defined(ENABLE_UART)
    UART_Init();
    #endif
}



#ifdef ENABLE_USB_CDC
static void putAndRetry(uint8_t *data, uint16_t len){
uint32_t retries = 1000;
	while(retries--){
		if(	CDC_Transmit_FS(data, len) == USBD_OK)
			break;
	}
}

static void stdout_enqueue_char(char c){
	putAndRetry((uint8_t*)&c, 1);
}

static void stdout_puts(const char *s){
uint16_t len = 0;
	
	while( *((const char*)(s + len)) != '\0'){
		len++;	
	}
	putAndRetry((uint8_t*)s, len);
}
#endif

#if defined(ENABLE_UART) 


void UART_Init(void){
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;

    pinInit(UART_TX_PIN, GPO_AF | GPO_2MHZ);  // TX
    pinInit(UART_RX_PIN, GPI_PU);            // RX

    USART1->BRR = 0x271;        //115200
    USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

    //USART1->CR1 |= USART_CR1_UE;
    //while((USART1->SR & USART_SR_TC) == 0);
    //NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );  
    HAL_NVIC_SetPriority(USART1_IRQn, IRQ_PRIORITY_LOW ,0);
    NVIC_EnableIRQ(USART1_IRQn);
}

static void stdout_enqueue_char(char c){
    xQueueSendToBack(stdout_queue, &c, pdMS_TO_TICKS(100));
    USART1->CR1 |= USART_CR1_TXEIE;
}

static void stdout_puts(const char *s){
    while(*s){
        xQueueSendToBack(stdout_queue, s++, pdMS_TO_TICKS(100));
    }
    USART1->CR1 |= USART_CR1_TXEIE;
}

void USART1_IRQHandler(void){
volatile uint32_t status = USART1->SR;
uint8_t data;

    // Data received
    if (status & USART_SR_RXNE) {
        USART1->SR &= ~USART_SR_RXNE;
        stdin_queue_char((uint8_t*)&USART1->DR);
    }
    
    // Check if data transmiter if empty 
    if (status & USART_SR_TXE) {
        USART1->SR &= ~USART_SR_TXE;	          // clear interrupt
        // Check if data is available to send
        if(xQueueReceiveFromISR( stdout_queue, &data, NULL) == pdPASS){        
            USART1->DR = data;            
        }else{
            // No more data, disable interrupt
            USART1->CR1 &= ~USART_CR1_TXEIE;      // disable TX interrupt if nothing to send
        }
    }    
}
#endif

#if defined(ENABLE_USB_CDC) || defined(ENABLE_UART)
StdOut stdio_ops = {
    .init = stdio_init,
    .xgetchar = stdin_wait_char,
    .xputchar = stdout_enqueue_char,
    .xputs = stdout_puts,
    .getCharNonBlocking = stdin_try_dequeue,
    .kbhit = stdin_queued
};
#endif

/**
 * ADC Driver
 * */

#define ADC_TRIGGER_CLOCK 500000UL
#define ADC_CR1_DUALMOD_FAST_INTERLEAVED     (7 << 16)
#define ADC_CR1_DUALMOD_SIMULTANEOUS         (6 << 16)
#define ADC_SQR1_L_(len)                     ((len) << 20)
#define ADC_SQR3_SQ1_(ch)                    (ch << 0)
#define ADC_SQR3_SQ2_(ch)                    (ch << 5)
#define ADC_SQR3_SQ3_(ch)                    (ch << 10)
#define ADC_SQR3_SQ4_(ch)                    (ch << 15)
#define ADC_SQR3_SQ5_(ch)                    (ch << 20)
#define ADC_SMPR2_SMP0_(cy)                  (cy << 0)
#define ADC_SMPR2_SMP1_(cy)                  (cy << 3)
#define ADC_SMPR2_SMP2_(cy)                  (cy << 6)
#define ADC_SMPR2_SMP3_(cy)                  (cy << 9)
#define ADC_SMPR2_(ch, cy)                   (cy << (ch * 3))
#define ADC_CH_IN(ch)                        (0xf << (ch*4))
#define ADC_VREFINT_CHANNEL                  17
#define ADC_VREFINT_VALUE                    1200.0f
#define ADC_CR2_EXTSEL_SWSTART               (15 << 17)

typedef struct {
    struct {
        uint8_t cal : 1;
        uint8_t res : 1;
    }flags;
    uint32_t calibration_code;
    float resolution;
    void (*cb)(uint16_t);
}adchandle_t;

static adchandle_t hadc;

/**
 * @brief
 * */
uint16_t ADC_GetCalibration(void){
    return hadc.calibration_code;
}

/**
 * @brief
 * */
float ADC_GetResolution(void){
    return hadc.resolution;
}


/**
 * @brief Configure sample time for a channel
 * */
static void adcSampleTime(uint16_t ch, uint16_t time){
    if(ch > 17){  // Max 17 channels
        return;
    }

    if(ch < 10){
        // Sample time for channels AN9-0
        ADC1->SMPR2 =  (ADC1->SMPR2 & (7 << (3 * ch))) | (time << (3 * ch));
    }else{
        // Sample time for channels AN17-10
        ADC1->SMPR1 =  (ADC1->SMPR1 & (7 << (3 * (ch % 10)))) | (time << (3 * (ch % 10)));
    }
}
/**
 * @brief Perform adc internal calibration and 
 * calculate resolution based on the 1.20V 
 * internal reference
 * */
void ADC_Calibrate(void){
uint32_t cr1, sqr1, sqr3;

    //Backup registers
    sqr3 = ADC1->SQR3;
    sqr1 = ADC1->SQR1;
    cr1 = ADC1->CR1;
    // Clear Interrupts
    ADC1->CR1 = 0;

    // Perform ADC calibration
    ADC1->CR2 |= ADC_CR2_CAL;
    while(ADC1->CR2 & ADC_CR2_CAL){               
        __asm volatile("nop");
    }

    hadc.calibration_code = ADC1->DR;
    // Set calibration flag
    hadc.flags.cal = 1;    
    // select VREFINT channel for first conversion
    ADC1->SQR3 = (ADC_VREFINT_CHANNEL << 0);
    // Ensure one conversion
    ADC1->SQR1 = 0; 
    // wake up Vrefint 
    ADC1->CR2 |= ADC_CR2_TSVREFE;
    // wait power up
    for(int t = 1000; t > 0; t--){
        __asm volatile("nop");
    }
    // Start conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while(!(ADC1->SR & ADC_SR_EOC)){
        __asm volatile("nop");
    }
    // Compute resolution
    hadc.resolution = ADC_VREFINT_VALUE / ADC1->DR;
    // power down VREFINT
    ADC1->CR2 &= ~ADC_CR2_TSVREFE;
    //restore registers
    ADC1->SQR3 = sqr3;
    ADC1->SQR1 = sqr1;
    ADC1->CR1 = cr1;
    // Set resolution flag
    hadc.flags.res = 1;
}
#ifndef USE_ADCMGR
static void (*eotcb)(uint16_t*);
// Each index holds two conversion results
static uint32_t adcres[ADC_SEQ_LEN];
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
    DMA1_Channel1->CNDTR =  ADC_SEQ_LEN;        // We are going to transfer 32-bit, 2 convertions at once (V,I)
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

    /* Configure ADC 1 voltage measurements*/
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
                ADC_CR1_SCAN;               // Scan through all channels on sequence

    ADC1->SQR1 = ADC_SQR1_L_(ADC_SEQ_LEN - 1);            // number of channels on sequence
    ADC1->SQR3 = ADC_SQR3_SQ1_(ADC_CH_VOLTAGE1) |  // First convertion OUT voltage, second load voltage
                 ADC_SQR3_SQ2_(ADC_CH_VOLTAGE2) |
                 ADC_SQR3_SQ3_(ADC_CH_VOLTAGE3) |
                 ADC_SQR3_SQ4_(ADC_CH_VOLTAGE4) |
                 ADC_SQR3_SQ5_(ADC_CH_V_LOAD);

    ADC1->SMPR2 = ADC_SMPR2_(ADC_CH_VOLTAGE1, 7) |      // set sample time to 239.5 cycles
                  ADC_SMPR2_(ADC_CH_VOLTAGE2, 7) | 
                  ADC_SMPR2_(ADC_CH_VOLTAGE3, 7) | 
                  ADC_SMPR2_(ADC_CH_VOLTAGE4, 7) | 
                  ADC_SMPR2_(ADC_CH_V_LOAD, 7);
                
    /* Configure ADC 2 current measuments*/
    RCC->APB2ENR  |= RCC_APB2ENR_ADC2EN;    // Enable Adc2
    RCC->APB2RSTR |= RCC_APB2ENR_ADC2EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_ADC2EN;

    ADC2->CR2 = ADC_CR2_EXTTRIG  |
    		    ADC_CR2_EXTSEL_2 |          // Select SWSTART for dual convertion
				ADC_CR2_EXTSEL_1 |
    		    ADC_CR2_EXTSEL_0 |
				ADC_CR2_ADON;

    ADC2->SQR1 = ADC_SQR1_L_(ADC_SEQ_LEN - 1);           // number of channels on sequence
    ADC2->SQR3 = ADC_SQR3_SQ1_(ADC_CH_CURRENT) |         // first convertion CH1, second CH3 
                 ADC_SQR3_SQ2_(ADC_CH_CURRENT) |
                 ADC_SQR3_SQ3_(ADC_CH_CURRENT) |
                 ADC_SQR3_SQ4_(ADC_CH_CURRENT) |
                 ADC_SQR3_SQ5_(ADC_CH_I_LOAD);

    ADC2->SMPR2 = ADC_SMPR2_(ADC_CH_CURRENT, 7) |      // CH1 and CH3 sample time, 239.5 cycles                  
                  ADC_SMPR2_(ADC_CH_I_LOAD, 7);

    ADC2->CR1 = ADC_CR1_SCAN;

    /* Configure Analog Pins */
    GPIOA->CRL &= ~(ADC_CH_IN(ADC_CH_VOLTAGE1) |
                  ADC_CH_IN(ADC_CH_CURRENT) |
                  ADC_CH_IN(ADC_CH_V_LOAD) |
                  ADC_CH_IN(ADC_CH_I_LOAD));

    NVIC_SetPriority(DMA1_Channel1_IRQn, 0); // Highest priority
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    eotcb = NULL;                           // No callback configured
    TIM2->CR1 |= TIM_CR1_CEN;
}


uint16_t *ADC_LastConvertion(void){
    return (uint16_t*)adcres;
}

/**
 * DMA transfer interrupt handler
 * */
void DMA1_Channel1_IRQHandler(void){

    if(DMA1->ISR & DMA_ISR_TCIF1){
        ADC1->SR = 0;                    // Clear ADC1 Flags
        ADC2->SR = 0;
            if(eotcb != NULL)
        eotcb((uint16_t*)adcres);
    }

    DMA1->IFCR = DMA_IFCR_CGIF1;    // Clear DMA Flags TODO: ADD DMA Error handling ?
}

void ADC_SetCallBack(void (*cb)(uint16_t*)){
    eotcb = cb;
}
#else /* USE_ADCMUX */
/* ***********************************************************
 * @brief ADC is triggered by TIM2 TRGO and performs single convertion 
 * on one channel. The result is placed on destination using the EOC interrupt
 * 
 * @param ms : Time between convertions
 ************************************************************ */
void ADC_Init(uint16_t ms){
    #if 0
    /* Configure Timer 2 */
    RCC->APB1ENR  |= RCC_APB1ENR_TIM2EN;    // Enable Timer 2
    RCC->APB1RSTR |= RCC_APB1ENR_TIM2EN;    // Reset timer registers
    RCC->APB1RSTR &= ~RCC_APB1ENR_TIM2EN;

    TIM2->CCMR1 = (3<<TIM_CCMR1_OC2M_Pos);  // Toggle OC2REF on match
    TIM2->CCER = TIM_CCER_CC2E;             // Enable compare output for CCR2
    TIM2->PSC = (SystemCoreClock/10000) - 1; // Timer counts 100us units

    TIM2->ARR = (ms * 5) - 1;               // Due to OC2REF toggle, timer must count half of 100us units
    TIM2->CCR2 = TIM2->ARR;

    /* Configure ADC 1, single convertion and EOC interrupt */
    RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN;     // Enable Adc1
    RCC->APB2RSTR |= RCC_APB2ENR_ADC1EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_ADC1EN;

    ADC1->CR2 = ADC_CR2_ADON;               // Turn on ADC1
    ADC1->CR2 |=
            ADC_CR2_EXTTRIG  |              // Only the rising edge of external signal can start the conversion
            //ADC_CR2_EXTSEL_2 |            // 0b100 Select TIM3_TRGO as Trigger source
            ADC_CR2_EXTSEL_1 |              // 0b011 Select TIM2_CC2 Event
            ADC_CR2_EXTSEL_0 ;              //

    ADC1->CR1 = ADC_CR1_EOCIE;

    ADC1->SQR1 = ADC_SQR1_L_(1 - 1);                // number of channels on sequence
    ADC1->SQR3 = ADC_SQR3_SQ1_(ADCMUX_CHANNEL);     // Set channel to be converted                
    ADC1->SMPR2 = ADC_SMPR2_(ADCMUX_CHANNEL, 7);    // set sample time to 239.5 cycles

    g_adceoc = NULL;                           // No callback configured

    NVIC_EnableIRQ(ADC1_2_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;
    #else
    /* Configure ADC 1, single convertion with EOC interrupt */
    RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN;        // Enable Adc1
    RCC->APB2RSTR |= RCC_APB2ENR_ADC1EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_ADC1EN;

    ADC1->CR2  = ADC_CR2_ADON;                  // Turn on ADC1
    ADC1->CR2 |= ADC_CR2_EXTTRIG  |             // required for sw start
                 ADC_CR2_EXTSEL_SWSTART;        // Software start
                
    adcSampleTime(ADC_VREFINT_CHANNEL, 3);      // Sample time 3 => 28.5 cycles.
    adcSampleTime(ADCMUX_CHANNEL, 7);           // set sample time to 239.5 cycles

    // Perform start up calibration
    ADC_Calibrate();
    
    ADC1->CR1 = ADC_CR1_EOCIE;                  // Enable end of convertion interrupt

    ADC1->SQR1 = ADC_SQR1_L_(1 - 1);            // number of channels on sequence
    ADC1->SQR3 = ADC_SQR3_SQ1_(ADCMUX_CHANNEL); // Set channel to be converted                
    

    hadc.cb = NULL;                             // No callback configured

    NVIC_EnableIRQ(ADC1_2_IRQn);
    #endif
}

void ADC_SetCallBack(void (*cb)(uint16_t)){
    while(ADC1->SR & ADC_SR_EOC);
    hadc.cb = cb;
}

void ADC_Start(void){
    ADC1->CR2 |= ADC_CR2_SWSTART;
}
/**
 * 
 * */
void ADC1_2_IRQHandler(void){
    if(ADC1->SR & ADC_SR_EOC){
        uint16_t result = ADC1->DR;
        if(hadc.cb != NULL)
            hadc.cb(result);
    }
    //DBG_PIN_TOGGLE;
}
#endif
/**
 * RTC
 * 
 */
#define RTC_ONE_SECOND_PRESCALER        0x7FFF
#define RTC_TIMEOUT                     1000
void RTC_Init(void){
uint32_t cnt;

    PWR->CR |= PWR_CR_DBP;                           // Disable write protection on Backup domain control register

    RCC->BDCR |= RCC_BDCR_LSEON;                     // Enable external 32.768khz oscillator

    cnt = RTC_TIMEOUT;
    while(!(RCC->BDCR & RCC_BDCR_LSERDY)){           // wait for oscillator to be ready
        if(--cnt == 0) return;
    }

    RCC->BDCR |= RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_0; // Select and enable clock to RTC

    PWR->CR &= ~PWR_CR_DBP;                          // Enable write protection on Backup domain control register

    RTC->CRL |= RTC_CRL_CNF;                         // Enter  configuration

    RTC->PRLL = RTC_ONE_SECOND_PRESCALER;

    RTC->CRL &= ~RTC_CRL_CNF;                        // Exit configuration
}


/**
 * SPI API
 * This board uses SPI2
 * 
 * Pins:
 * PB12 -> CS
 * PB13 -> SCLK
 * PB14 <- MISO used for LCD_CD
 * PB15 -> MOSI
 * 
 * */
#define DMA_CCR_PL_Medium   (1<<12)
#define DMA_CCR_MSIZE_8     (0<<10)
#define DMA_CCR_MSIZE_16    (1<<10)
#define DMA_CCR_MSIZE_32    (2<<10)
#define DMA_CCR_PSIZE_16    (1<<8)
#define DMA_CCR_PSIZE_8     (0<<8)

typedef struct spihandle{
    DMA_Channel_TypeDef *dma;
    volatile uint32_t count;
}spihandle_t;

static spihandle_t spi2handle;

void SPI_Init(void){
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    RCC->APB1RSTR |= RCC_APB1RSTR_SPI2RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    
    GPIOB->CRH &= (0x0000FFFF);
    GPIOB->CRH |= (0xA2A20000);  // AF-PP
    
    SPI2->CR1 = (1 << 3) |       //BR[2:0] = PLK1/4 
                //SPI_CR1_SSM |
                //SPI_CR1_DFF |
                SPI_CR1_MSTR;

    SPI2->CR2 = SPI_CR2_SSOE;

    SPI2->CR1 |= SPI_CR1_SPE;


    spi2handle.count = 0;
    spi2handle.dma = DMA1_Channel5;  //Channel3 for SPI1

    spi2handle.dma->CPAR = (uint32_t)&SPI2->DR;
    spi2handle.dma->CCR = 
                DMA_CCR_PL_Medium |
                DMA_CCR_MSIZE_16  |
                DMA_CCR_PSIZE_16  |
                DMA_CCR_TCIE      |
                DMA_CCR_DIR;

    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}

uint8_t SPI_Send(uint8_t data){
    SPI2->DR = data;
	while((SPI2->SR & SPI_SR_TXE) == 0 );
	while((SPI2->SR & SPI_SR_BSY) != 0 );
    return SPI2->DR;
}

void SPI_Read(uint8_t *dst, uint32_t len){

}

/**
 * @brief Sends a block of data through the spi bus using dma
 * block. Due to dma limitation, data is transffered in 64k blocks when len grater
 * than 0x10000.
 * 
 * @param src : pointer to data to be sent
 * @param len : size of data, if bit 32 is set then src[0] will be transffered len times
 * */
void SPI_WriteDMA(uint16_t *src, uint32_t len){

    // Configure Spi for 16bit DMA
    SPI2->CR1 &= ~SPI_CR1_SPE;
    SPI2->CR1 |= SPI_CR1_DFF | SPI_CR1_SPE;

    if(len & 0x80000000){
        len &= 0x7FFFFFFF;
        spi2handle.dma->CCR &= ~(DMA_CCR_MINC);
    }else{
        spi2handle.dma->CCR |= DMA_CCR_MINC;
    }

    SPI2->CR2 |= SPI_CR2_TXDMAEN;

    spi2handle.count = len;    
    spi2handle.dma->CMAR = (uint32_t)src;
    spi2handle.dma->CNDTR = (spi2handle.count > 0x10000) ? 0xFFFF : spi2handle.count;
    
    spi2handle.dma->CCR |= DMA_CCR_EN;
}

/**
 * @brief End of transfer callback
 * */
__weak void spi_eot(void){

}

/**
 * DMA handler for spi transfers
 * */
void DMA1_Channel5_IRQHandler(void){

    if(DMA1->ISR & DMA_ISR_TCIF5){
        spi2handle.dma->CCR &= ~(DMA_CCR_EN);
        
        if(spi2handle.count > 0x10000){
            spi2handle.count -= 0x10000;
            spi2handle.dma->CNDTR = (spi2handle.count > 0x10000) ? 0xFFFF : spi2handle.count;
            spi2handle.dma->CCR |= DMA_CCR_EN;
        }else{
            // wait for the last byte to be transmitted
            while(SPI2->SR & SPI_SR_BSY){
                if(SPI2->SR & SPI_SR_OVR){
                    //clr OVR flag
                    spi2handle.count = SPI2->DR;
                }
            }
            /* Restore 8bit Spi */
	        SPI2->CR1 &= ~(SPI_CR1_SPE | SPI_CR1_DFF);
	        SPI2->CR1 |= SPI_CR1_SPE;
            SPI2->CR2 &= ~(SPI_CR2_TXDMAEN);
            spi2handle.count = 0;
            spi_eot();
        }
    }
    DMA1->IFCR = DMA_IFCR_CGIF5;
}

/**
* @brief I2C MSP Initialization
* This function configures the hardware resources used in this example
* PB10     ------> I2C2_SCL
* PB11     ------> I2C2_SDA 
* @param hi2c: I2C handle pointer
* @retval None
*/
void I2C_Init(void){

    __HAL_RCC_GPIOB_CLK_ENABLE();
    //gpioInit(GPIOB, 10, GPO_10MHZ | GPO_AF | GPO_OD);
    //gpioInit(GPIOB, 11, GPO_10MHZ | GPO_AF | GPO_OD);
    GPIOB->BSRR = (1<<11) | (1<<10);
    GPIOB->CRH = (GPIOB->CRH & ~(15 << 8)) | (0xFF << 8);
    __HAL_RCC_I2C2_CLK_ENABLE();
    HAL_NVIC_SetPriority(I2C2_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
    HAL_NVIC_SetPriority(I2C2_ER_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);

    hi2c2.Instance = I2C2;
    hi2c2.Init.ClockSpeed = 100000;
    hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if(HAL_I2C_Init(&hi2c2) != HAL_OK) {
        Error_Handler(__FILE__, __LINE__);
    }
}

void I2C_Write(uint8_t addr, uint8_t *data, uint32_t size){
    HAL_I2C_Master_Transmit(&hi2c2, addr << 1, data, size, 100);
}

void I2C_Read(uint8_t addr, uint8_t *data, uint32_t size){
    HAL_I2C_Master_Receive(&hi2c2, addr << 1, data, size, 100);
}


/**
 * @brief Flash write functions for EEPROM emulation
 */
uint32_t flashWrite(uint8_t *dst, uint8_t *data, uint16_t count){
uint16_t *src = (uint16_t*)data;
uint32_t res, address = (uint32_t)dst;

    res = HAL_FLASH_Unlock();
    if( res == HAL_OK){    
        for (uint16_t i = 0; i < count; i+= 2, src++){
            res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address + i, *src);
            if(res != HAL_OK){
                break; 
            }
        }
    }
    HAL_FLASH_Lock();
    return res;
}

/**
 * @brief Erase 1k selctor on flash
 * 
 * @param address:  start address for erasing
 * @return : 0 on fail
 * */
uint32_t flashPageErase(uint32_t address){
uint32_t res;

    res = HAL_FLASH_Unlock();

    if( res == HAL_OK){
        FLASH_PageErase(address);
    }
    
    HAL_FLASH_Lock();
    return 1;
}

/**
 * */
void Error_Handler(const char *file, int line){
    while(1){

    }
}
