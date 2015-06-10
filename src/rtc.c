
#include <timers.c>

unsigned int _miliseconds, _seconds;
char _secondsChange;

//--------------------------------------------
//
//--------------------------------------------
void startCounting(void)
{
	stopTimer1;
	_miliseconds = 1000;
	_seconds = 0;
	_secondsChange = 0;
	setTimer1(1000);  // Interrupt every 1ms
	startTimer1;
}
//--------------------------------------------
//
//--------------------------------------------
void stopCounting(void)
{
	stopTimer1;
}
//--------------------------------------------
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
void interrupt handler(void)
{
	if(CCP1IF)
	{
		_miliseconds--;
		if(!_miliseconds)
		{
			_seconds++;
			_miliseconds = 1000;
			_secondsChange = 1;
		}
		CCP1IF = 0;
		//RC0 = _miliseconds & 1;
	}
}
