/*********************************************
PSU V2.0.0
**********************************************/

#include "common.h"
#include "selector.h"

#define TITLE "PIC PSU V2.0"
bank1 string title = { 
	TITLE,
	sizeof(TITLE),
	INVERTED,
	(LCD_W/2)-(sizeof(TITLE)*(FONT_W/2)),
	0
};

void powerSupply(void);
void electronicLoad(void);

struct MenuItem modes[]={
	{" PSU ",0,powerSupply},		//power Supply	
	{" Lod ",0,electronicLoad},	//constant load
	{" Bat ",0,powerSupply},		//battery discharge curve an capacity
	{" OPT ",0,powerSupply}		//
};
//------------------------------------------------------
//
//------------------------------------------------------
void main(void)
{
	systemInit();	
	drawFrame(&title);	
	while (!done()){
		//((void(*)(void))(changeMode(modes,sizeof(modes)/sizeof(struct MenuItem))->data))();
		changeMode(modes,sizeof(modes)/sizeof(struct MenuItem))->run();
		clrCanvas();
	}
}

