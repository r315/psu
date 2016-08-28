
#include "common.h"
#include "rtc.h"

#define LD_MENU_ITEMS sizeof(LD_menu)/sizeof(menuitem)
void LD_changePwm(void);
void LD_changeLimit(void);
void LD_start(void);

bank1 uchar LD_pwmval=254;
bank1 uint LD_limitval = 1200;
bank1 static menuitem LD_menu[] = {
	{" MIN ",0,LD_changeLimit},
	{" AMP ",0,LD_changePwm},	
	{" STA ",0,LD_start},
	{" Exit ",0,0}
};

void checkEnd(void){
char time[10];
	//if vsel > vmesure
	//if imesured < 2-5ma
	//stopCounting();
	//getTime(time);
	//printText(90,5,time);
}

void LD_start(void){
//setDuty(loadctrl->channel, loadctrl->duty);
}

void LD_changePwm(void){
	updateValueForKey(MAX_IOUT_PWM_VAL, MIN_IOUT_PWM_VAL, &LD_pwmval);
	printDecimal(90,3,NORMAL_DIGIT, LD_pwmval*ICONST,1000,0);
}

void LD_changeLimit(void){
	switch(getKey()){
		case L_KEY:
			if(LD_limitval > MIN_VOUT)
				LD_limitval -= 100;
			break;	
	
		case R_KEY:
			if(LD_limitval < MAX_VOUT)
				LD_limitval += 100;
			break;	
	}	
	printDecimal(90,2,NORMAL_DIGIT, LD_limitval/100,100,10);
}

void electronicLoad(void){
menuitem *item;
uchar keys = M_KEY, updateTime = 0;

	enableLoad(1);
	drawSetsUnits();
	
	do{
		if(!updateTime){			
			updateDro();			
			checkEnd();		
			lcdUpdate();
			updateTime = 20;
		}
		updateTime--;

		if(keys){
			if(keys == M_KEY){
				item = selectMenuItem(LD_menu, LD_MENU_ITEMS);			
				if(!item->run) return;	
				drawMenuItem(item);
			}
			//drawSetIcon(77, 2+((uint)item - (uint)LD_menu)/sizeof(menuitem));
			//item->run();
			//Error: recursive function calls: 
	//_electronicLoad -> ARG(_electronicLoad) -> indirect(88) -> indirect(88) -> 
			updateTime = 0;
		}
		keys = scanKeys();
		delayMs(20);
	}while(item->run);
}

