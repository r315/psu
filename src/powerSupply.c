#include "common.h"

#define PS_MENU_SPACING (FONT_W*8)
#define PS_MENU_ITEMS sizeof(psmenu)/sizeof(menuitem)
#define PS_VOLTAGE_COL 10
#define PS_VOLTAGE_PAGE 2
#define PS_CURRENT_COL 10
#define PS_CURRENT_PAGE 4

void psFrame(void);
void printVoltage(unsigned int v);
void printCurrent(unsigned int c);

bank1 static pwm voutpwm = {MINVOUT,VSET_CH,MINVOUT,MAXVOUT},ioutpwm = {MINIOUT,ISET_CH,MINIOUT,MAXIOUT};
bank1 static mesure psout;
bank1 static menuitem psmenu[3] = {
	{"Voltage",&voutpwm},
	{"Current",&ioutpwm},
	{"Exit",0}
};

void powerSupply(void){
uchar updateTime = 0, done = M_KEY;	
menuitem *item;
pwm *pwmch;

	psFrame();	
	enableLoad(0);
	do{
		if(!updateTime){			
			getMesures(&psout);
			printVoltage(PS_VOLTAGE_COL, PS_VOLTAGE_PAGE,psout.ch1_voltage);			
			printCurrent(PS_CURRENT_COL, PS_CURRENT_PAGE,psout.current);
			printDecimal(88,5,NORMAL_DIGIT,psout.power,100,10);
			lcdUpdate();
			updateTime = 20;
		}
		updateTime--;

		if(done){
			if(done == M_KEY){
				item = selectMenuItem(psmenu,PS_MENU_ITEMS,PS_MENU_SPACING);				
				pwmch = (pwm*)(item->data);
				if(!pwmch) return;
				drawMenuItem(item);	
			}
			setDuty(pwmch->channel,pwmch->duty);
			printDecimal(88,2,NORMAL_DIGIT,(psout.adc_ch1*VCONST)/100,100,10);
			printDecimal(88,3,NORMAL_DIGIT,ioutpwm.duty * ICONST,1000,0);
			updateTime = 0;			
		}
		done = readKeysUpdate(pwmch->maxduty,pwmch->minduty,&pwmch->duty);		
		delayMs(20);
	}while(item->data);
}

//------------------------------------------------------
//      
//                  |set       
//            v     |     V    
//                  |     A    
//            mA    +----------
//                  |          
//                  |
//
//------------------------------------------------------
const char bmSET[]={174,170,186,128,190,170,162,128,130,190,130,128};
void psFrame(void)
{
#define LFRAMESIZ 80
unsigned char i;
	
	for(i=1; i < 7 ;i++){
		lcdsetPos(LFRAMESIZ,i);// linha V separadora de frames
		lcdData(255);
	}

	lcdsetPos(LFRAMESIZ+1,4);	
	for(i=LFRAMESIZ+1; i< LCD_W-1 ;i++){// linha H separadora de frames
		lcdData(0x10);
	}	
	
	lcdsetPos(LFRAMESIZ+1,1);  // bitmap "SET"
	for(i = 0; i < sizeof(bmSET); i++){
		lcdData(~bmSET[i]);
	}	
	printText(48,2,"V");
	printText(54,4,"mA");	
	printText(113,2,"V");
	printText(113,3,"mA");
	printText(113,5,"W");
}
