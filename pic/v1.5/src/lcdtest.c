/*********************************************
V1.1
TODO:
registo de currente maxima
stopwatch
**********************************************/
#define FULLFONT 
#define BPORT   PORTB 
#define BPORTMask 0x0F
#define L_KEY (1<<2)
#define R_KEY (1<<0)
#define M_KEY (1<<1)

#define LOAD_EN RC0

#define LCDSDI RC5
#define LCDCLK RC3
#define LCDRS  RC4
#define LCDRST RC6

#define VMENU 0
#define AMENU 1
#define LMENU 2

//#define PIC_CLK 8000000

#include <picboard.c>
#include <spi.c>
#include "lcd.c"


//------------------------------------------------------
//
//------------------------------------------------------
void main(void)
{
char update;
unsigned int mesuredVoltage;
unsigned int mesuredCurrent;

//	systemInit(DIGITAL);
	ADCON0 = 0b11000001;  // Frc clock | ADON
	ADCON1 = 0b00000011;  // RA3 = ref, AN0,1,2,5
	OPTION &= ~0x80;  // enable pull-ups on PORTB
	TRISA = 0xFF;
	TRISB = 0xF0;
	TRISC = 0b10000011;
	PORTC = 0;

	spiInit(SPI_MODE3);
	lcdInit();
	
	
 while(1)
 {		
	if(update++ > 5)
	{
		
	}	 
	DelayMs(100);		
 }  
}
