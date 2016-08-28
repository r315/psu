
#include "common.h"
#include "rtc.h"

#define LD_MENU_ITEMS sizeof(loadmenu)/sizeof(menuitem)

bank1 static pwm loadpwm = {MINIOUT,ISET_CH,MINIOUT,MAXIOUT};
bank1 static menuitem loadmenu[] = {
	{" AMPS ",&loadpwm},
	//{" AMPS ",&loadpwm},
	{" Start ",&loadpwm},
	{" Exit ",0}
};

void electronicLoad(void){
menuitem *item;
uchar done = M_KEY, updateTime = 0;
pwm *loadctrl;
char time[10];	

	enableLoad(1);
	startCounting();
	do{
		if(!updateTime){			
			updateDro();			
			stopCounting();
			getTime(time);
			printText(90,5,time);
			lcdUpdate();
			updateTime = 20;
		}
		updateTime--;

		if(done){
			if(done == M_KEY){
				item = selectMenuItem(loadmenu,LD_MENU_ITEMS);			
				loadctrl = (pwm*)(item->data);
				if(!loadctrl) return;	
				drawMenuItem(item);
			}
			setDuty(loadctrl->channel,loadctrl->duty);
			printDecimal(90,4,NORMAL_DIGIT,loadctrl->duty*ICONST,1000,0);
			updateTime = 0;
		}
		done = readKeysUpdate(loadctrl->maxduty,loadctrl->minduty,&loadctrl->duty);		
		delayMs(20);
	}while(item->data);
}

