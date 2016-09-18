
#include "common.h"
#include "rtc.h"

enum LD_States{
	S_CUT_OFF_SEL = 1,
	S_DISCHARGE,
	S_DISCH_SEL,
	S_IDLE,
	S_PAUSE,
	S_START,
	S_STOP
};

#define LD_MENU_ITEMS sizeof(LD_menu)/sizeof(menuitem)
#define LD_DISCHARGE_MENU_ITEMS sizeof(LD_dischargemenu)/sizeof(menuitem)

bank1 uchar LD_pwmval= MIN_IOUT;
bank1 uint LD_cutoff = 0;
bank1 uchar LD_state = S_IDLE;

bank1 static menuitem LD_menu[] = {
	{" Cut-Off voltage ",(void*)S_CUT_OFF_SEL},
	{" Discharge Current ",(void*)S_DISCH_SEL},	
	{" Start ",(void*)S_START},	
	{" Exit ",(void*)0}
};

bank1 static menuitem LD_dischargemenu[] = {
	{" Resume ",(void*)S_PAUSE},
	{" Stop ",(void*)S_STOP},	
};

void drawLoadSetsValues(void){		
	printDecimal(90,2,NORMAL_DIGIT, LD_cutoff/100,100,10);	
	printDecimal(90,3,NORMAL_DIGIT, LD_pwmval*ICONST,1000,0);	
}

char LD_StateProcess(void){
char time[10];
menuitem *item;
uchar keys = scanKeys();

switch(LD_state){

	case S_IDLE:		
		item = selectMenuItem(LD_menu, LD_MENU_ITEMS);			
		if(!item->data){
			setDuty(ISET_CH, MIN_IOUT);
			return ON; //done
		}				
		clrMenu();
		LD_state = (uchar)item->data;
		clrSetIcon(77,2);
		clrSetIcon(77,3);		
		break;

	case S_CUT_OFF_SEL:
		switch(keys){
				case L_KEY:
					if(LD_cutoff > 0)
						LD_cutoff -= 100;
					break;	
	
				case R_KEY:
					if(LD_cutoff < MAX_VOUT)
						LD_cutoff += 100;
					break;	
				case M_KEY:
					LD_state = S_IDLE;
					return OFF;
			}
			drawSetIcon(77, 2);
			drawLoadSetsValues();
			break;

	case S_DISCH_SEL:
			if(updateValueForKey(MAX_IOUT_PWM_VAL, MIN_IOUT_PWM_VAL, &LD_pwmval) == M_KEY){
				LD_state = S_IDLE;
				break;
			}
			drawSetIcon(77, 3);
			drawLoadSetsValues();
			break;

	case S_START:
			drawLoadSetsValues();
			setDuty(ISET_CH, LD_pwmval);
			startCounting();
			LD_state = S_DISCHARGE;
			printTextAtr(MENU_START_COL,MENU_PAGE,"Discharging!",UNDERLINE);
			break;

	case S_DISCHARGE:
		if(outvalues.ch1_voltage < LD_cutoff){
			setDuty(ISET_CH, MIN_IOUT);
			stopCounting();
			clrMenu();
			printTextAtr(MENU_START_COL,MENU_PAGE,"End!",UNDERLINE);
			while(!scanKeys());
			LD_state = S_IDLE;
			break;
		}

		if(timeChange()){
			getTime(time);
			printText(90,5,time);
		}

		if(keys == M_KEY){
			item = selectMenuItem(LD_dischargemenu, LD_DISCHARGE_MENU_ITEMS);
			switch((uchar)item->data){
				case S_STOP: 
					LD_state = S_STOP;
					break;
				case S_PAUSE:
					clrMenu();
					break;		
			}
			printTextAtr(MENU_START_COL,MENU_PAGE,"Discharging!",UNDERLINE);
		}
		break;	
		
	case S_PAUSE:
		break;
	case S_STOP:
		setDuty(ISET_CH, MIN_IOUT);
		LD_state = S_IDLE;
		break;
}
return OFF;
}

void electronicLoad(void){
uchar updateTime = 0;
	enableLoad(ON);
	drawSetsUnits();	
	LD_state = S_IDLE;
	do{
		if(!updateTime){			
			updateDro();			
			lcdUpdate();			
			updateTime = 50;
		}
		updateTime--;		
		//delayMs(20);
	}while(!LD_StateProcess());
	setDuty(ISET_CH, MIN_IOUT);
}

