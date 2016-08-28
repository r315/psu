#include "common.h"

#define PS_MENU_ITEMS sizeof(psmenu)/sizeof(menuitem)

bank1 static pwm voutpwm = {MIN_VOUT_PWM_VAL,VSET_CH,MIN_VOUT_PWM_VAL,MAX_VOUT_PWM_VAL},ioutpwm = {MIN_IOUT_PWM_VAL,ISET_CH,MIN_IOUT_PWM_VAL,MAX_IOUT_PWM_VAL};
bank1 static menuitem psmenu[3] = {
	{" Volt ",&voutpwm},
	{" Amps ",&ioutpwm},
	{" Main ",0}
};

void powerSupply(void){
uchar updateTime = 0, done = M_KEY;	
menuitem *item = 0;
pwm *pwmch;
unsigned int a,b;

	enableLoad(0);
	drawSetsUnits();
	do{
		if(!updateTime){			
			updateDro();
			lcdUpdate();
			updateTime = 20;
		}
		updateTime--;

		if(done){
			if(done == M_KEY){
				if(item)
					clrSetIcon(77, 2+((uint)item - (uint)psmenu)/sizeof(menuitem));
				item = selectMenuItem(psmenu,PS_MENU_ITEMS);				
				pwmch = (pwm*)(item->data);
				if(!pwmch) return;
				drawMenuItem(item);				
				drawSetIcon(77, 2+((uint)item - (uint)psmenu)/sizeof(menuitem));
			}
			setDuty(pwmch->channel,pwmch->duty);
			printDecimal(90,2,NORMAL_DIGIT,(outvalues.adc_ch1*VCONST)/100,100,10); //set voltage is read directly from regulator
			printDecimal(90,3,NORMAL_DIGIT,ioutpwm.duty * ICONST,1000,0);      //set current is calculated based on pwm reg
			updateTime = 0;			
		}
		done = readKeysAndUpdateValue(pwmch->maxduty,pwmch->minduty,&pwmch->duty);		
		delayMs(20);
	}while(item->data);
}
