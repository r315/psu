/*********************************************
V1.1
TODO:

**********************************************/
#define FULLFONT 
//#define DEBUG

#define MINVOUT 22
#define MAXVOUT 255
#define MINIOUT 0
#define MAXIOUT 255

#include <picboard.c>
#include <spi.c>
#include "lcd.c"
#include <pwm.c>
#include <adc.c>
#include "special.c"
#include "screen.c"
#include "rtc.c"
#include "menu.c"

const char TITLE[] = {"PIC PSU V1.51"};
char setvout, setiout, menu,update;
//------------------------------------------------------
//
//------------------------------------------------------
void main(void)
{
unsigned int mesuredVoltage;
unsigned int mesuredCurrent;
unsigned int calculatedPower;

	systemInit(ANALOG);
	ADCON0 = 0b11000001;  // Frc clock | ADON
	ADCON1 = 0b00000011;  // RA3 = ref, AN0,1,2,5
	OPTION &= ~0x80;      // enable pull-ups on PORTB
	TRISA = 0b11111111;
	TRISB = TRISB_VALUE;
	TRISC = TRISC_VALUE;
	PORTB = 255; // 0?

	spiInit(SPI_MODE3); //added on 03/08/2016
	lcdInit();
	rtcInit();
	
	setvout = MINVOUT;
    setiout = MINIOUT;
	setFpwm(PWMFREQ);	
	setDuty(ISET_CH,setiout);
	setDuty(VSET_CH,setvout);
	
	menu = VMENU;
	drawFrame();
	psFrame();
	setFontPatern(INVERTED);
	printString((LCD_W/2)-(sizeof(TITLE)*FONT_W/2),0,TITLE);
	drawMenu();	
	LOAD_EN = 0;
	update = 20;	
	
 while(1)
 {		
	if(update++ > 5)
	{
		//TODO: circuito V2 pcb
		mesuredVoltage = getVout(VOUT_CH1);// - getVout(VOUT_CH2);
		mesuredCurrent = getIout();
		calculatedPower = mesuredVoltage/100;
		calculatedPower *= (mesuredCurrent/10);
		
		printVoltage(mesuredVoltage);
		printCurrent(mesuredCurrent);	
		if(menu != LMENU)
		{
			setFontPatern(NORMAL);
			printDval(WATTAGE_C,WATTAGE_P,calculatedPower);printc('W');printc(' ');		
			printDval(SET_VOLTAGE_C,SET_VOLTAGE_P,(setvout*VCONST));printc('V');printc(' ');			
			printDval(SET_CURRENT_C,SET_CURRENT_P,(setiout*ICONST));
		}
		else
		{
			// TODO: adicionar fim de descarga v<x
			if(hasUpdate()){
				setFontPatern(NORMAL);
				printDval(WATT_C1,WATT_P1,calculatedPower);printc('W');printc(' ');
				lcdsetPos(WATT_C1,3);
				getTime(_outstr);
				printString(WATT_C1,3,_outstr);
				if( !(getSeconds() % 60) ){
					addgraphData(mesuredCurrent/100);
					updateGraph();	
				}				
			}
		}		
		update = 0;
	}	
	butoes();
	DelayMs(20);		
 }  
}

