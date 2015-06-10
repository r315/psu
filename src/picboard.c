#include "picboard.h"
#include "delay.c" 
//------------------------------------------------
// PICF73
//------------------------------------------------
void systemInit(char Digital)
{
	if(Digital) 
		ADCON1 = 0b00000110;  // Digital IO only
	else
	{
		ADCON0 = 0b11000001;  // Frc clock | ADON
		ADCON1 = 0b00000000;  // Digital and Analog IO, VDD as ref
	}
}
//------------------------------------------------
//
//------------------------------------------------
char keydown(char k)
{
	return (~BPORT) & k & BPORTMask;
}
//------------------------------------------------
//
//------------------------------------------------
char scanKeys(void)
{
	return (~BPORT) & BPORTMask;
}
