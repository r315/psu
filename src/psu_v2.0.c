/*********************************************
PSU V2.0.0
**********************************************/

#include "common.h"
#include "selector.h"

static const char title[]= {"PIC PSU V2.1"};

void powerSupply(void);
void electronicLoad(void);
void batDischarge(void);

struct MenuItem modes[]={
	{" PSU ",0,powerSupply},			//power Supply	
	{" Load ",0,electronicLoad},		//constant load	
	{" BAT ",0,batDischarge}	    	//battery discharge
};


void printInt(char c, char p, unsigned int val){
	printDecimal(c, p, NORMAL_DIGIT, val, 1000, 0);
}

void BUTTON_Test(void){
char pressed = 0, hold = 0;
while(1){
	scanKeys();
	
	switch(scanKeysState()){
		
		case BUTTON_PRESSED:
			printInt(0,0,++pressed);
			break;
			
		case BUTTON_HOLD:
			printInt(0,1,++hold);
			break;
			
		case BUTTON_RELEASED:
			hold = 0;
			pressed = 0;
			printInt(0,0,pressed);
			printInt(0,1,hold);
			break;
	}

	delayMs(100);
}
}

//------------------------------------------------------
//
//------------------------------------------------------
void main(void)
{
	systemInit();	
	drawFrame(title,(LCD_W/2)-(sizeof(title)*(FONT_W/2)));	
	updateDro();
	
	//BUTTON_Test();
	while (!done()){
		//((void(*)(void))(changeMode(modes,sizeof(modes)/sizeof(struct MenuItem))->data))();
		changeMode(modes,sizeof(modes)/sizeof(struct MenuItem))->run();
		disableOutput();
		clrSetsArea();
	}
}

