
#include "board.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "usbd_cdc_if.h"
#include <stdout.h>


void BOARD_Init(void){
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;
    
    EXPANDER_Init();
    RTC_Init();
    LED_INIT;

    SPI_Init();
    LCD_Init();
    LCD_Rotation(LCD_REVERSE_LANDSCAPE);
}

void setInterval(void(*cb)(), uint32_t ms){
    // start loop, timer is configures on startup
    // and call is made on interupt handler
    HAL_TIM_Base_Start_IT(&htim4);
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

static void (*eotcb)(uint16_t*);
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

    eotcb = NULL;                           // No callback configures
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

    SPI2->CR2 = SPI_CR2_SSOE | SPI_CR2_TXDMAEN;

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

void SPI_WriteDMA(uint16_t *src, uint32_t len){

    // Configure Spi for 16bit DMA
    SPI2->CR1 &= ~SPI_CR1_SPE;
    //SPI2->CR2 |= SPI_CR2_TXDMAEN;
    SPI2->CR1 |= SPI_CR1_DFF | SPI_CR1_SPE;

    if(len & 0x80000000){
         len &= 0x7FFFFFFF;
    }else{
        spi2handle.dma->CCR |= DMA_CCR_MINC;
    }

    spi2handle.count = len;    
    spi2handle.dma->CMAR = (uint32_t)src;
    spi2handle.dma->CNDTR = (spi2handle.count > 0x10000) ? 0xFFFF : spi2handle.count;  // transfer blocks of 64k
    
    spi2handle.dma->CCR |= DMA_CCR_EN;

    while(spi2handle.count);
}

void DMA1_Channel5_IRQHandler(void){

    if(DMA1->ISR & DMA_ISR_TCIF5){
        spi2handle.dma->CCR &= ~(DMA_CCR_EN | DMA_CCR_MINC);
        
        if(spi2handle.count > 0x10000){
            spi2handle.count -= 0x10000;
            spi2handle.dma->CNDTR = (spi2handle.count > 0x10000) ? 0xFFFF : spi2handle.count;
            spi2handle.dma->CCR |= DMA_CCR_EN;
        }else{
            /* Restore 8bit Spi */
	        SPI2->CR1 &= ~(SPI_CR1_SPE | SPI_CR1_DFF);
	        //SPI2->CR2 &= ~SPI_CR2_TXDMAEN;
	        SPI2->CR1 |= SPI_CR1_SPE;
            // wait for the last byte to be transmitted
            while(SPI2->SR & SPI_SR_BSY){
                if(SPI2->SR & SPI_SR_OVR){
                    //clr OVR flag
                    spi2handle.count = SPI2->DR;
                }
            }
            spi2handle.count = 0;
        }
    }
    DMA1->IFCR = DMA_IFCR_CGIF5;
}