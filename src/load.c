
#include "common.h"

#define LD_MENU_SPACING (FONT_W*5)
#define LD_MENU_ITEMS sizeof(loadmenu)/sizeof(menuitem)
#define LD_VOLTAGE_COL 12
#define LD_VOLTAGE_PAGE 1
#define LD_CURRENT_COL 66
#define LD_CURRENT_PAGE 1

bank1 static pwm loadpwm = {MINIOUT,ISET_CH,MINIOUT,MAXIOUT};
bank1 static menuitem loadmenu[] = {
	{"Set",&loadpwm},
	{"Exit",0}
};
void loadFrame(void);

void electronicLoad(void){
menuitem *item;
uchar done = M_KEY, updateTime = 0;
pwm *loadctrl;
mesure ldout;

	loadFrame();
	enableLoad(1);
	do{
		if(!updateTime){			
			getMesures(&ldout);
			printVoltage(LD_VOLTAGE_COL, LD_VOLTAGE_PAGE,ldout.ch1_voltage);			
			printCurrent(LD_CURRENT_COL, LD_CURRENT_PAGE,ldout.current);
			printDecimal(96,3,NORMAL_DIGIT,ldout.power,100,10);
			lcdUpdate();
			updateTime = 20;
		}
		updateTime--;

		if(done){
			if(done == M_KEY){
				item = selectMenuItem(loadmenu,LD_MENU_ITEMS,LD_MENU_SPACING);			
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

void loadFrame(void){
unsigned char i;
	
	for(i=1; i < 3 ;i++){
		lcdsetPos(59,i);// linha V separadora de frames
		lcdData(255);
	}

	lcdsetPos(1,3);	
	for(i=1; i< LCD_W-1 ;i++){// linha H
		lcdData(0x1);
	}	
	printText(48,1,"V");
	printText(110,1,"mA");	
	printText(120,3,"W");
	printText(114,4,"mA");
}
