
#include "system.h"
#include "rtc.h"



unsigned int _seconds;
char _secondsChange;
uint32_t _ticks;
//--------------------------------------------
//
//--------------------------------------------
void rtcInit(void){
	
}
//--------------------------------------------
//
//--------------------------------------------
void startCounting(void)
{
	_ticks = SDL_GetTicks();
	_seconds = 0;
	  
}
//--------------------------------------------
//
//--------------------------------------------
void stopCounting(void)
{
	_seconds = (SDL_GetTicks() - _ticks) / 1000;
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
	*(dst + 5) = '\0';	
	//*(dst + 5) = ':';	
	//getDigits(dst + 6,_seconds%60);
	//*(dst + 8) = '\0';	
}
//------------------------------------------*/
//
//--------------------------------------------
int getSeconds(void)
{
	return _seconds;
}
//--------------------------------------------
//
//--------------------------------------------
char hasUpdate(void)
{
uint32_t secondsnow = (SDL_GetTicks() - _ticks) / 1000;
	return secondsnow - _seconds;
}

