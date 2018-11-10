/********************************************************
Tosc = osc/4
PR2 = (Tosc/(Fpwm*Pres))-1 <=> PR2 = ((Tosc/Fpwm)/Pres)-1
*******************************************************/   
#include "system.h"
#include "pwm.h"

void setFpwm(unsigned int fpwm)
{
	char pres = 16;
	unsigned int tmp;

	TMR2ON = 0;            // Stop Timer
	CCP1CON = CCP_PWM;     // select pwm mode CCP1
	CCP2CON = CCP_PWM;     // select pwm mode CCP2

	while(pres){           // calcula Prescaller e valor de PR2
		tmp = Tosc/fpwm;
		tmp /= pres; 
		PR2 = tmp-1;
		if(PR2 > MIN_PR2)
			break;
		pres /= 4;
	}

	switch(pres){	
		case 1 : T2CON = 0;break;// 00 /1
		case 4 : T2CON = 1;break;// 01 /4
		case 16: T2CON = 2;break;// 1x /16
		default: T2CON = 0;break;// default prescaller 1
	}	
	
	TRISC2 = 0;
	TRISC1 = 0;	
	TMR2ON = 1;
}
//---------------------------------------------------
// 
//---------------------------------------------------
void setDuty(char ch, int duty)
{
unsigned char tmp;	
	if(!ch)
	{
		CCPR1L = duty>>2;                // 8 msb bits	
		tmp = duty & 3;
		CCP1CON = (tmp<<4)|CCP_PWM;      // 2 lsb bits
	}
	else
	{
		CCPR2L = duty>>2;                // 8 msb bits	
		tmp = duty & 3;
		CCP2CON = (tmp<<4)|CCP_PWM;      // 2 lsb bits
	}
}



