
#include "system.h"
#include "rtc.h"



unsigned int _seconds;
char _secondsChange;
char RTC_isrunning = 0;
uint32_t _ticks;
//--------------------------------------------
//
//--------------------------------------------
void rtcInit(void){
	RTC_isrunning = false;
	_ticks = SDL_GetTicks();
	_seconds = 0;
}
//--------------------------------------------
//
//--------------------------------------------
int peekTime(void){
	if(RTC_isrunning)
		_seconds = (SDL_GetTicks() - _ticks) / 1000;
	return _seconds;
}
//--------------------------------------------
//
//--------------------------------------------
void startCounting(void)
{
	_ticks = SDL_GetTicks();
	_seconds = 0;
	RTC_isrunning = true;
}
//--------------------------------------------
//
//--------------------------------------------
void stopCounting(void)
{
	RTC_isrunning = false;
}
//------------------------------------------*/
//
//--------------------------------------------
int getSeconds(void)
{
	return peekTime();	
}
//--------------------------------------------
//
//--------------------------------------------
char timeChange(void)
{
	uint32_t secondsnow;
	if(!RTC_isrunning) return false;		
	secondsnow = (SDL_GetTicks() - _ticks) / 1000;
	if(secondsnow - _seconds)
		return peekTime();
	return false;
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
	#if 0 //use seconds	
	*(dst + 5) = ':';	
	getDigits(dst + 6,_seconds%60);
	*(dst + 8) = '\0';	
	#else
	*(dst + 5) = '\0';	
	#endif
}
