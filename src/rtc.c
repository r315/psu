
#include "system.h"
#include "rtc.h"

bank1 unsigned int _seconds;
bank1 char _secondsChange, _ticks;
//--------------------------------------------
//
//--------------------------------------------
void rtcInit(void){
	OPTION &= 0xC0;
	OPTION |=
     (0<<5)  // CLK = CLKOUT (T0CS)
    |(1<<4)  // increment timer on rising edge (T0SE)
    |(0<<3)  // PSA Assign prescaler to timer0
    |(1<<2)  // PS2 1:256
    |(1<<2)  // PS1
    |(1<<0); // PS0   	
    PEIE = 1;	
	GIE = 1;
}
//--------------------------------------------
//
//--------------------------------------------
void startCounting(void)
{
	_ticks = CONST_1S;
	_seconds = 0;
	TMR0IE = 1;   
}
//--------------------------------------------
//
//--------------------------------------------
void stopCounting(void)
{
	TMR0IE = 0; 
}
//--------------------------------------------
//
//--------------------------------------------
void getDigits(char *dst, char t){
	if(t < 10){
		dst[0] = '0';
		dst[1] = t + '0';
	}else{
		dst[1] = '0' + (t % 10);
		t /= 10;
		dst[0] = '0' + (t % 10);
	}
}
//--------------------------------------------
//
//--------------------------------------------
void getTime(char *dst){
char aux;
unsigned int tmp;
	aux = _seconds/3600;
	getDigits(dst,aux);
	*(dst + 2) = ':';	
	tmp = _seconds-(aux*3600);	
	aux = tmp/60;
	getDigits(dst + 3,aux);
	*(dst + 5) = ':';	
	getDigits(dst + 6,_seconds%60);
	*(dst + 8) = '\0';	
}
//------------------------------------------*/
//
//--------------------------------------------
int getSeconds(void)
{
	_secondsChange = 0;
	return _seconds;
}
//--------------------------------------------
//
//--------------------------------------------
char hasUpdate(void)
{
	return _secondsChange;
}
//--------------------------------------------
//interrupt every ms, count seconds
//--------------------------------------------
void interrupt handler(void){
	if(TMR0IF )
	{
		_ticks--;
		if(!_ticks)
		{
			_seconds++;
			_ticks = CONST_1S;
			_secondsChange = 1;
			#ifdef DEBUG
			DBG_PIN ^= 1;
			#endif			
		}		
		TMR0IF  = 0;
	}
}
