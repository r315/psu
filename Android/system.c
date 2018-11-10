
#include <htc.h>
#include <pwm.h>

#define I2C_ADDR	(0x63<<1)   // 8-bit slave address
#define I2C_MODE	0x06	    // 7-bit address, slave mode
#define I2C_ENABLE	(1<<5)
#define SCL_ENABLE	(1<<4)
#define ADC_CFG0 0xC1    		// Frc, ADON

#if defined(_16F819)
__CONFIG(0x3F30);/*
RA0 <- V1    RB0 -> LED
RA1 <- I1    RB1 <-> SDA
RA2 - io     RB2 -> PWM
RA3 <- AN3   RB3 - io
RA4 - io     RB4 -> SCL
RA5 - MCLR   RB5 - io
RA6 - io     RB6 - PGC
RA7 - io     RB7 - PGD */
#define PIC_CLK     8000000
#define GPI_MASK 0b11010100
#define GPI_PORT PORTA
#define GPO_MASK 0b00000001 //SDA, SCL inputs
#define GPO_PORT PORTB
#define OSC_CFG 0x70    // 8Mhz
#define ADC_CFG1 0x84   // Right justified,AN0,AN1,AN3

void systemInit(void){
	OSCCON = OSC_CFG;
	ADCON0 = ADC_CFG0;   
	ADCON1 = ADC_CFG1; 
	TRISA = ~GPI_MASK;
	TRISB = ~GPO_MASK;
    PORTB = 255;
    PORTA = 255;
}
#endif

#if defined(_16F73) || defined(_16F76)
__CONFIG(0x3FBA);/*
RA0 <- V1     RB7 <- I
RA1 <- I1     RB6 <- I
RA3 <- Vref   RB5 <- I
RC1 -> PWM2   RB4 <- I
RC2 -> PWM1   RB3 -> O
RC3 -> SCL    RB2 -> O
RC4 <-> SDA   RB1 -> O
              RB0 -> O*/
#define PIC_CLK     12000000
#define GPI_MASK 0b11110000
#define GPI_PORT PORTB
#define GPO_MASK 0b00001111
#define GPO_PORT PORTB
#define ADC_CFG1 0x05   // AN0,AN1, Vref

void systemInit(void){
	ADCON0 = ADC_CFG0;
	ADCON1 = ADC_CFG1;
	TRISA = 0b11111111;
	TRISB = GPI_MASK;
	TRISC = 0b11111111;
    PORTB = 255;
    PORTA = 255;
	PORTC = 255;
}
#endif
//---------------------------------
//
//---------------------------------
void i2cSetup(void){
	SSPSTAT &= 0b00111111; // clr SMP, CKE
	SSPCON = I2C_ENABLE|I2C_MODE|SCL_ENABLE;	
	SSPADD = I2C_ADDR;
	
	SSPIE = 1;
	PEIE = 1;
	GIE = 1;	
}
//---------------------------------
//
//---------------------------------
void pwmSetup(void){
	TMR2ON = 0;
	PR2 = 255;	
	CCP1CON = CCP_PWM;
	#if defined(_19F73) || defined(_16F76)
	CCP2CON = CCP_PWM;	
	TRISC2 = 0;
	#endif
	TRISC1 = 0;
	TMR2ON = 1;
}
//---------------------------------
//
//---------------------------------
void getFunctions(int *dst){
	#if defined(_19F819)
	*dst = 
			 (2	<< 0)	//AN channels
			|(1 << 2)	//PWM channels
			|(1	<< 4)	//output ports
			|(1	<< 6)	//input ports
	;
	#else
	*dst = 
			 (3	<< 0)	//AN channels
			|(2 << 2)	//PWM channels
			|(1	<< 4)	//output ports
			|(1	<< 6)	//input ports
	;
	#endif
}
//---------------------------------
//
//---------------------------------
void setAdc(int *src){
char ch = *src >> 10;	// skip 10 bits
	ch &= 3;
	ADCON0 = ADC_CFG0;	// clear current channel select	
	ADCON0 |= (ch<<3);	// apply the new channel select
}

void getAdc(int *dst){	
	#if defined(_16F819)
	GODONE = 1;	
	while(GODONE);	
	*dst = (ADRESH<<8) | ADRESL;
	#else
	ADGO = 1;	
	while(ADGO);	
	*dst = ADRES;
	#endif
}

void setPwm(int *src){
	#if defined(_16F819)
	setDuty(*src);
	#else
	char ch = *src >> 10;
	ch &= 3;
	setDuty(ch,*src);
	#endif
}

void getPwm(int *src){
// implementar se necessario
}

void getGpi(int *dst){
	*dst = GPI_PORT & GPI_MASK;
}

void setGpo(int *src){
	PORTB &= ~GPO_MASK;
	PORTB |= *src & GPO_MASK;
}

void dummyFunction(int *dmy){

}
