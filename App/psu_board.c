
#include "board.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "usbd_cdc_if.h"
#include <stdout.h>

#if defined(ENABLE_I2C)
static I2C_HandleTypeDef hi2c2;
#endif

void BOARD_Init(void){
    
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;
    
    LED_INIT;
    PSU_OE_INIT;
    MUX_SEL_INIT;
    SPOWER_INIT;
    DBG_PIN_INIT;
        
    (LCD_SPIDEV)->bus = SPI_BUS1;
    (LCD_SPIDEV)->freq = 40000;
    (LCD_SPIDEV)->cfg = SPI_SW_CS;
    SPI_Init(LCD_SPIDEV);

    /**
     * PB15 AF
     * PB14 GPO/CD
     * PB13 AF
     * PB12 GPO/CS
     * PB3  GPO/BKL
     * */
    GPIOB->BSRR = (5 << 12); // CS, RS
    GPIOB->BRR  = (1 << 3);  // BKL

    GPIOB->CRH &= (0x0000FFFF);
    GPIOB->CRH |= (0xA2A20000);

    GPIOB->CRL &= (0xFFFF0FFF);
    GPIOB->CRL |= (0x00002000);

#if defined(ENABLE_I2C)
    I2C_Init();
#endif
    RTC_Init();
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
 * 
 * \param initial - startup duty for each channel
 * 
 * */
void PWM_Init(uint16_t pwm1, uint16_t pwm2, uint16_t pwm3){

     /* Configure Timer 3 */
    RCC->APB1ENR  |= RCC_APB1ENR_TIM3EN;
    RCC->APB1RSTR |= RCC_APB1ENR_TIM3EN;
    RCC->APB1RSTR &= ~RCC_APB1ENR_TIM3EN;

    TIM3->CR1 = TIM_CR1_ARPE;
    TIM3->CCMR1 = (6<<TIM_CCMR1_OC2M_Pos) | (6<<TIM_CCMR1_OC1M_Pos) | (TIM_CCMR1_OC2PE) | (TIM_CCMR1_OC1PE);  // PWM Mode 1
    TIM3->CCMR2 = (6<<TIM_CCMR2_OC3M_Pos) | (TIM_CCMR2_OC3PE); 
    TIM3->CCER = TIM_CCER_CC3E | TIM_CCER_CC2E | TIM_CCER_CC1E;
    TIM3->PSC = 1; // Timer3 freq = SystemClock / 2

    TIM3->ARR = PWM_MAX_VALUE - 1;
    TIM3->CCR1 = pwm1;
    TIM3->CCR2 = pwm2;
    TIM3->CCR3 = pwm3;
    
    TIM3->CR1 |= TIM_CR1_CEN;     // Start pwm before enable outputs

    /* Configure PWM pins */
    GPIOB->CRL &= ~(0xFF000F << 0); // PB5-PB4, PB0
    GPIOB->CRL |= (0xAA000A << 0);
    // remap PB5-PB4
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
#if defined(ENABLE_CLI)
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
#endif

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
#define ADC_TRIGGER_CLOCK                    500000UL
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
    ADC_TypeDef *adc;
    struct {
        uint8_t cal : 1;
        uint8_t res : 1;
    }flags;
    uint32_t calibration_code;
    float resolution;
    void (*cb)(uint16_t);
}adchandle_t;

static adchandle_t hadc1, hadc2;

/**
 * @brief
 * */
uint16_t ADC_GetCalibration(void){
    return hadc1.calibration_code;
}

/**
 * @brief Return weight step in mV
 * */
float ADC_GetResolution(void){
    return hadc1.resolution;
}


/**
 * @brief Configure sample time for a channel
 * */
static void adcSampleTime(ADC_TypeDef *adc, uint16_t ch, uint16_t time){
    if(ch > 17){  // Max 17 channels
        return;
    }

    if(ch < 10){
        // Sample time for channels AN9-0
        adc->SMPR2 =  (adc->SMPR2 & (7 << (3 * ch))) | (time << (3 * ch));
    }else{
        // Sample time for channels AN17-10
        adc->SMPR1 =  (adc->SMPR1 & (7 << (3 * (ch % 10)))) | (time << (3 * (ch % 10)));
    }
}
/**
 * @brief Perform adc internal calibration and 
 * calculate resolution based on the 1.20V 
 * internal reference
 * */
static void adc_calibrate(adchandle_t *hadc){
uint32_t cr1, cr2, sqr1, sqr3;

    //Backup registers
    sqr3 = hadc->adc->SQR3;
    sqr1 = hadc->adc->SQR1;
    cr1 = hadc->adc->CR1;
    cr2 = hadc->adc->CR2;
    // Clear Interrupts
    hadc->adc->CR1 = 0;
    hadc->adc->CR2 = ADC_CR2_ADON | ADC_CR2_EXTTRIG  |  // required for sw start
                     ADC_CR2_EXTSEL_SWSTART;            // Software start

    // Perform ADC calibration
    hadc->adc->CR2 |= ADC_CR2_CAL;
    while(hadc->adc->CR2 & ADC_CR2_CAL){               
        __asm volatile("nop");
    }

    hadc->calibration_code = hadc->adc->DR;
    // Set calibration flag
    hadc->flags.cal = 1;    
    // select VREFINT channel for first conversion
    hadc->adc->SQR3 = (ADC_VREFINT_CHANNEL << 0);
    // Ensure one conversion
    hadc->adc->SQR1 = 0; 
    // wake up Vrefint 
    hadc->adc->CR2 |= ADC_CR2_TSVREFE;
    // wait power up
    for(int t = 1000; t > 0; t--){
        __asm volatile("nop");
    }
    // Start conversion
    hadc->adc->CR2 |= ADC_CR2_SWSTART;
    while(!(hadc->adc->SR & ADC_SR_EOC)){
        __asm volatile("nop");
    }
    // Compute resolution
    hadc->resolution = ADC_VREFINT_VALUE / hadc->adc->DR;
    // power down VREFINT
    hadc->adc->CR2 &= ~ADC_CR2_TSVREFE;
    //restore registers
    hadc->adc->SQR3 = sqr3;
    hadc->adc->SQR1 = sqr1;
    hadc->adc->CR1 = cr1;
    hadc->adc->CR2 = cr2;
    // Set resolution flag
    hadc->flags.res = 1;
}

void ADC_Calibrate(void){
    adc_calibrate(&hadc1);
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
#if defined(TIMED_ADC)
/* ***********************************************************
 * @brief ADC is triggered by TIM2 TRGO and performs single convertion 
 * on ADCMUX_CHANNEL. 
 * 
 * Upon conversion the EOC flag is set and interrupt handler is called,
 * if a EOC callback is configured, it is called passing the conversion result
 * 
 * @param ms : Time between convertions
 ************************************************************ */
void ADC_Init(uint16_t ms){
    /* Configure Timer 2 */
    RCC->APB1ENR  |= RCC_APB1ENR_TIM2EN;    // Enable Timer 2
    RCC->APB1RSTR |= RCC_APB1ENR_TIM2EN;    // Reset timer registers
    RCC->APB1RSTR &= ~RCC_APB1ENR_TIM2EN;

    TIM2->CCMR1 = (3<<TIM_CCMR1_OC2M_Pos);  // Toggle OC2REF on match
    TIM2->CCER = TIM_CCER_CC2E;             // Enable compare output for CCR2
    TIM2->PSC = (SystemCoreClock/10000) - 1;// Timer counts 100us units

    TIM2->ARR = (ms * 5) - 1;               // Due to OC2REF toggle, timer must count half of 100us units
    TIM2->CCR2 = TIM2->ARR;

    /* Configure ADC 1, single convertion and EOC interrupt */
    RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN;     // Enable Adc1
    RCC->APB2RSTR |= RCC_APB2ENR_ADC1EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_ADC1EN;

    hadc1.adc = ADC1;

    ADC1->CR2  = ADC_CR2_ADON;               // Turn on ADC1
    ADC1->CR2 |= ADC_CR2_EXTTRIG  |          // Only the rising edge of external signal can start the conversion
                 //ADC_CR2_EXTSEL_2 |        // 0b100 Select TIM3_TRGO as Trigger source
                 ADC_CR2_EXTSEL_1 |          // 0b011 Select TIM2_CC2 Event
                 ADC_CR2_EXTSEL_0 ;          //

    adcSampleTime(hadc1.adc, ADC_VREFINT_CHANNEL, 3);  // Sample time 3 => 28.5 cycles.
    adcSampleTime(hadc1.adc, ADC_MUX_CH, 7);           // set sample time to 239.5 cycles
    
    // Perform start up calibration
    adc_calibrate(&hadc1);

    ADC1->CR1 = ADC_CR1_EOCIE;

    ADC1->SQR1 = ADC_SQR1_L_(1 - 1);            // number of channels on sequence
    ADC1->SQR3 = ADC_SQR3_SQ1_(ADC_MUX_CH);     // Set channel to be converted                

    hadc1.cb = NULL;                            // No callback configured    

    NVIC_EnableIRQ(ADC1_2_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;

    /* ADC2 Init for reading power button */
    RCC->APB2ENR  |= RCC_APB2ENR_ADC2EN;
    RCC->APB2RSTR |= RCC_APB2ENR_ADC2EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_ADC2EN;

    ADC2->CR2  = ADC_CR2_ADON;                  // Turn on ADC
    ADC2->CR2 |= ADC_CR2_EXTTRIG  |             // required for sw start
                 ADC_CR2_EXTSEL_SWSTART;        // Software start

    hadc2.adc = ADC2;

    adcSampleTime(hadc2.adc, ADC_PWR_SW_CH, 7);    // set sample time to 239.5 cycles

    ADC2->SQR1 = ADC_SQR1_L_(1 - 1);            // number of channels on sequence
    ADC2->SQR3 = ADC_SQR3_SQ1_(ADC_PWR_SW_CH);  // Set channel to be converted
}

void ADC_Start(void){
    TIM2->CR1 |= TIM_CR1_CEN;
}

void ADC_Stop(){
    TIM2->CR1 &= ~(TIM_CR1_CEN);
}
#else /* TIMED_ADC */
/** 
* @brief ADC 1 is triggered by software and performs a single conversion on 
* the channel defined by ADCMUX_CHANNEL.
* After the conversion the EOC flag is set and interrupt handler is called,
* if a callback is set, it is called with the conversion result
* 
* \param ms : not used
* */
void ADC_Init(uint16_t ms){
    /* Configure ADC 1, single convertion with EOC interrupt */
    RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN;        // Enable Adc1
    RCC->APB2RSTR |= RCC_APB2ENR_ADC1EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_ADC1EN;

    hadc1.adc = ADC1;

    ADC1->CR2  = ADC_CR2_ADON;                  // Turn on ADC
    ADC1->CR2 |= ADC_CR2_EXTTRIG  |             // required for sw start
                 ADC_CR2_EXTSEL_SWSTART;        // Software start
                
    adcSampleTime(hadc1.adc, ADC_VREFINT_CHANNEL, 3);      // Sample time 3 => 28.5 cycles.
    adcSampleTime(hadc1.adc, ADC_MUX_CH, 7);           // set sample time to 239.5 cycles

    // Perform start up calibration
    adc_calibrate(&hadc1);
    
    ADC1->CR1 = ADC_CR1_EOCIE;                  // Enable end of convertion interrupt

    ADC1->SQR1 = ADC_SQR1_L_(1 - 1);            // number of channels on sequence
    ADC1->SQR3 = ADC_SQR3_SQ1_(ADC_MUX_CH);     // Set channel to be converted                
    
    hadc1.cb = NULL;                             // No callback configured    

    NVIC_EnableIRQ(ADC1_2_IRQn);


    /* ADC2 Init for reading power button */
    RCC->APB2ENR  |= RCC_APB2ENR_ADC2EN;
    RCC->APB2RSTR |= RCC_APB2ENR_ADC2EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_ADC2EN;

    ADC2->CR2  = ADC_CR2_ADON;                  // Turn on ADC
    ADC2->CR2 |= ADC_CR2_EXTTRIG  |             // required for sw start
                 ADC_CR2_EXTSEL_SWSTART;        // Software start

    hadc2.adc = ADC2;

    adcSampleTime(hadc2.adc, ADC_PWR_SW_CH, 7);    // set sample time to 239.5 cycles

    ADC2->SQR1 = ADC_SQR1_L_(1 - 1);            // number of channels on sequence
    ADC2->SQR3 = ADC_SQR3_SQ1_(ADC_PWR_SW_CH);  // Set channel to be converted              

}

void ADC_Start(void){
    ADC1->CR2 |= ADC_CR2_SWSTART;
}

void ADC_Stop(){

}
#endif /* TIMED_ADC */

void ADC_SetCallBack(void (*cb)(uint16_t)){
    while(ADC1->SR & ADC_SR_EOC);
    hadc1.cb = cb;
}

/**
 * 
 * */
void ADC1_2_IRQHandler(void){
    if(ADC1->SR & ADC_SR_EOC){
        uint16_t result = ADC1->DR;
        if(hadc1.cb != NULL)
            hadc1.cb(result);
    }
    //DBG_PIN_TOGGLE;
}
#endif /* USE_ADCMUX */

uint32_t ADC2_Read(uint8_t ch){
    ADC2->SQR3 = ADC_SQR3_SQ1_(ch);  // Set channel to be converted
    ADC2->CR2 |= ADC_CR2_SWSTART;

    while(!(ADC2->SR & ADC_SR_EOC));

    return ADC2->DR;
}


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
* @brief I2C MSP Initialization
* This function configures the hardware resources used in this example
* PB10     ------> I2C2_SCL
* PB11     ------> I2C2_SDA 
* @param hi2c: I2C handle pointer
* @retval None
*/
#if defined(ENABLE_I2C)
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
        BOARD_Error_Handler(__FILE__, __LINE__);
    }
}

uint16_t I2C_Write(uint8_t addr, uint8_t *data, uint32_t size){

    taskENTER_CRITICAL();
    {
        if(HAL_I2C_Master_Transmit(&hi2c2, addr << 1, data, size, 100) != HAL_OK){
            DBG_PRINT("Fail write to I2C\n");
            return 0;
        }
    taskEXIT_CRITICAL();
    }
    return size;
}

uint16_t I2C_Read(uint8_t addr, uint8_t *data, uint32_t size){
    
    taskENTER_CRITICAL();
    {
        if(HAL_I2C_Master_Receive(&hi2c2, addr << 1, data, size, 100) != HAL_OK){
            DBG_PRINT("Fail Read I2C\n");
            return 0;
        }
    }
    taskEXIT_CRITICAL();
    return size;
}
#endif
/**
 * @brief Configure watchdog timer according a given interval
 *  in wich the timer will expire and a system reset is performed
 * 
 * @param interval : Interval in wich the watchdog will perform a system reset
 * */
void enableWatchDog(uint32_t interval){
uint32_t timeout = 4096;
uint8_t pres = 0;

    interval *= 10;

    if(interval > 0xFFFF){
        interval = 0xFFFF;
    }    

    while( interval > timeout){
        timeout <<= 1;
        pres++;
    }

    if(IWDG->SR != 0){
        // other update is in progress
        return;
    }

    IWDG->KR = 0x5555; // Enable access to PR and RLR registers
    IWDG->PR = pres;
    IWDG->RLR = (interval * 0xFFFF) / timeout;
    IWDG->KR = 0xAAAA;  // Reload
    IWDG->KR = 0xCCCC;  // Start IWDG
}
/**
 * @brief Watchdog reset that mus be called before the interval
 *          specified on configuration
 * 
 * */
void reloadWatchDog(void){
    IWDG->KR = 0xAAAA; // Reload RLR on counter
}

/**
 * */
void BOARD_Error_Handler(const char *file, int line){
    while(1){

    }
}
