
#include "psu.h"


/**
 * HW modules configuration
 * TIM4   program loop tick
 * TIM2   ADC Trigger
 * DMA1   ADC data transfer
 * ADC1-2 Dual configuration for simultaneous convertion
 * TIM3   PWM signals
 * */

extern StdOut vcom;

class VoltageDro : public ConsoleCommand{
    double v = 0;
public:
    char execute(void *ptr){
        //SEVEN_Double(1,1,v);
        v += 0.5;
        return CMD_OK;
    }
    void help(void){}

    VoltageDro(void) : ConsoleCommand("vdro") {}
};

void UpdateResult(uint16_t *adcres){
static double c = 99.1;
    //SEVEN_Double(1,1, (3.3 * adcres[2]) / 4096);
    //SEVEN_Double(65,1, (3.3 * adcres[3]) / 4096);
    TEXT_dro(VOLTAGE_DRO_POS, c,1);
    c += 0.1;
    if(c == 100.0) c = 0;
    LCD_Update();
}

void handleButtons(void){
    BUTTON_Read();
    if(BUTTON_GetEvents() == BUTTON_PRESSED){
        switch(BUTTON_GetValue()){
            case BUTTON_LEFT:
                
                break;

            case BUTTON_RIGHT: 
                
                break;
        }
    }
}


extern const uint8_t icon_psu[];
extern const uint8_t icon_load[];
extern const uint8_t icon_out[];
void redrawDisplay(){

    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
    //TEXT_setFont(&defaultBoldFont);
    //TEXT_setFont(&lcdFont);
    TEXT_setFont(&pixelDustFont);
    //TEXT_setFont(&defaultFont);
    TEXT_print(0,0, "88.8W");
    TEXT_setFont(&font_seven_seg);
    TEXT_dro(VOLTAGE_DRO_POS, 88.8, 1);
    TEXT_dro(CURRENT_DRO_POS, 8.88, 2);

    TEXT_drawGfx(70,0, (uint8_t*)&icon_psu[0]);
    TEXT_drawGfx(90,0, (uint8_t*)&icon_load[0]);
    TEXT_drawGfx(110,0, (uint8_t*)&icon_out[0]);
    LCD_Update();

}

void cfgI2C_DMA(uint8_t *dst, uint16_t size){
    // Configure DMA1 CH4 to handle i2c transmission
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel4->CNDTR = size;
    DMA1_Channel4->CPAR = (uint32_t)&I2C2->DR;
    DMA1_Channel4->CMAR = (uint32_t)dst;    
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

/**
 * Called every 10ms by Timer4 interrupt, as console
 * may block the main thread, having a secondary loop
 * ensures operation of lcd update and button handling
 * */
void tskPsu(void *ptr){
 double c = 0.1;

 uint8_t buf[5];
    memset(buf, 0x44, 32);

    //I2C_Write(SSD1306_I2C_ADDRESS, buf, 0);
    cfgI2C_DMA(LCD_GetPixels() - 1, 512);
    
    //while(1){
    //    i2cSend(SSD1306_I2C_ADDRESS << 1, buf, sizeof(buf));
    //    vTaskDelay(200);
    //}
    

    TEXT_Init();
    redrawDisplay();
    //handleButtons();
     
    /*    switch(sup.mode){
            case PSU: break;
            case LOAD: break;
        }
    LCD_Update();*/
    while(1){
        HAL_GPIO_TogglePin(GPIOA, DBG_Pin);
        TEXT_dro(VOLTAGE_DRO_POS, c, 1);
        c += 0.1;
        if(c == 100.0) c = 0;
        LCD_Update();
        vTaskDelay(8);
    }
}

void tskConsole(void *ptr){
VoltageDro vdro;
Console console;
ConsoleHelp help;
CmdAdc adc1;
CmdPwm pwm;
CmdDfu dfu;
CmdPwr pwr;
    
    vcom.init();    
    console.init(&vcom, "PSU >");
    console.addCommand(&help);

    console.addCommand(&vdro);
    console.addCommand(&adc1);
    console.addCommand(&pwm);
    console.addCommand(&dfu);
    console.addCommand(&pwr); 

    while(1){
        console.process();
    }
}

extern "C" void psu(void){  

    //PWM_Init((uint16_t*)pwm_start_values);

    //ADC_Init(ADC_INTERVAL);
    //ADC_SetCallBack(UpdateResult);

    //setInterval(psu_v3_loop,APP_INTERVAL);

    xTaskCreate( tskConsole, "CLI", configMINIMAL_STACK_SIZE, NULL, 3, NULL );
    xTaskCreate( tskPsu, "PSU", configMINIMAL_STACK_SIZE * 2, NULL, 3, NULL );
}


