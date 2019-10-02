/*********************************************
V1.1
TODO:
registo de currente maxima
stopwatch
**********************************************/
#define FULLFONT 
#define BPORT   PORTC 
#define BPORTMask 0xF0
#define L_KEY (1<<6)
#define R_KEY (1<<4)
#define M_KEY (1<<5)

#define LOAD_EN RC0

#define LCDSDI RB3
#define LCDCLK RB2
#define LCDRS  RB1
#define LCDRST RB0

#define PWMFREQ 31300

#define VMENU 0
#define AMENU 1
#define LMENU 2

#define PIC_CLK 12000000

#include <picboard.c>
#include <ks0713.c>

const char TITLE[] = {"PIC PSU V1.5"};
char pwmvout, pwmiout, menu;

#include <pwm.c>
#include <adc.c>
#include "dac_adc.c"
#include "graficos.c"

//------------------------------------------------------
//
//------------------------------------------------------
void drawMenu(char m)
{	
	print(6,7,"                   ",UNDERLINE);
	switch(menu)
	{
		case VMENU:
			print(6,7," Volts ",INVERTED);			
			break;
		case AMENU:			
			print(6,7," Amps ",INVERTED);
			break;
		case LMENU:
			print(6,7," Load ",INVERTED);
			break;
	}	
}
//------------------------------
//
//------------------------------
void selectMode(void)
{	
	print(6,7," Volts  Amps  Load ",UNDERLINE);
	LOAD_EN = 0;
	while(scanKeys())DelayMs(10);
	
	while(1)
	{
		if(keydown(L_KEY))
		{
			menu = VMENU;			
			print(6,7," Volts ",INVERTED);
			drawBar(pwmvout);
			break;
		}
		if(keydown(M_KEY))
		{
			menu = AMENU;			
			print(6+(7*6),7," Amps ",INVERTED);
			drawBar(pwmiout);
			break;
		}
		if(keydown(R_KEY))
		{
			menu = AMENU;			
			print(6+(13*6),7," Load ",INVERTED);
			pwmiout = 0;
			LOAD_EN = 1;
			setDuty(ISET_CH,pwmiout);
			drawBar(pwmiout);
			break;
		}
	}
	DelayMs(250);
	while(scanKeys())DelayMs(10);
	drawMenu(menu);
}
//------------------------------
//
//------------------------------
void changeUp(void)
{
	switch(menu)
	{
		case VMENU:
			if(pwmvout < 255)
				setDuty(VSET_CH,++pwmvout);
				drawBar(pwmvout);
			break;
		case AMENU:
			if(pwmiout < 255)
				setDuty(ISET_CH,++pwmiout);
				drawBar(pwmiout);
			break;
		default: break;		
	}			
}
//------------------------------
//
//------------------------------
void changeDown(void)
{
	switch(menu)
	{
		case VMENU:
			if(pwmvout>MINVOUT)
				setDuty(VSET_CH,--pwmvout);
				drawBar(pwmvout);
			break;
		case AMENU:
			if(pwmiout>0)
				setDuty(ISET_CH,--pwmiout);
				drawBar(pwmiout);
			break;
		default: break;		
	}			
}
//------------------------------------------------------
//
//------------------------------------------------------
void main(void)
{
char update,i;

systemInit(ANALOG);
ADCON0 = 0b11000001;  // Frc clock | ADON
ADCON1 = 0b00000011;  // RA3 = ref, AN0,1,2,5
TRISA = 0xFF;
TRISB = 0xF0;
TRISC = 0xF0;

lcdInit();
drawFrame();
print((LCD_W/2)-(sizeof(TITLE)*FONT_W/2),0,TITLE,INVERTED);
drawBar(pwmvout);
drawMenu(menu);
LOAD_EN = 0;

setFpwm(PWMFREQ);

 while (1) 
 {	
	if(update++ > 5)
	{
		//printNumber(0,0,drawValueUnit(32,2,getVout(),VOLTS_SI),10);	
		printMesure(getVout(),VOLTS_SI);	
		printMesure(getIout(),mAMPS_SI);	
		update = 0;
	}
	
	if(keydown(L_KEY))
	{
		changeDown();		
		update = 20;
	}
	
	if(keydown(R_KEY))
	{
		changeUp();
		update = 20;
	}
	
	if(keydown(M_KEY))
	{		
		selectMode();		
	}
	DelayMs(100);		
 }  
}
//-------------------------------------------------------------------
//
//-------------------------------------------------------------------



