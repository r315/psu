/*********************************************
PSU V2.0.0
**********************************************/

#include "common.h"
#include "selector.h"

static const char title[]= {"PIC PSU V2.0"};

void powerSupply(void);
void electronicLoad(void);

struct MenuItem modes[]={
	{" PSU ",0,powerSupply},		//power Supply	
	{" Load ",0,electronicLoad}	//constant load	
};
//------------------------------------------------------
//
//------------------------------------------------------
void main(void)
{
	systemInit();	
	drawFrame(title,(LCD_W/2)-(sizeof(title)*(FONT_W/2)));	
	updateDro();
	while (!done()){
		//((void(*)(void))(changeMode(modes,sizeof(modes)/sizeof(struct MenuItem))->data))();
		changeMode(modes,sizeof(modes)/sizeof(struct MenuItem))->run();
		disableOutput();
		clrSetsArea();
	}
}

