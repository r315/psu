/*********************************************
PSU V2.0.0
**********************************************/

#include "common.h"
#include "selector.h"

#define TITLE "PIC PSU V2.0"
string title = { 
	TITLE,
	sizeof(TITLE),
	INVERTED,
	(LCD_W/2)-(sizeof(TITLE)*(FONT_W/2)),
	0
};

void powerSupply(void);

struct MenuItem modes[]={
	{" PSU ",0,powerSupply},		//power Supply
	{" Pow ",0,powerSupply},		//power curve
	{" Lod ",0,powerSupply},		//constant load
	{" Opt ",0,powerSupply}		//
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
	}
}

