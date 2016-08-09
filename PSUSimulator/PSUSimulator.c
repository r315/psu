
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "system.h"
#include "selector.h"

#define TITLE "PIC PSU V2.0.0"
const string title = { TITLE,sizeof(TITLE),INVERTED,(LCD_W/2)-(sizeof(TITLE)*(FONT_W/2)),0 };

void powerSupply(void);

struct MenuItem modes[]={
	{" PSU ",0,powerSupply},		//power Supply
	{" Pow ",0,powerSupply},		//power curve
	{" Lod ",0,powerSupply},		//constant load
	{" Opt ",0,powerSupply}		//
};

//------------------------------
//
//------------------------------
int main(int argc, char *argv[])
{
	lcdInit();
	drawFrame(&title);
	lcdUpdate();
	while (!done())
	{		
		//if (keyDown(M_KEY)){
			changeMode(modes,sizeof(modes)/sizeof(struct MenuItem))->run();			
			lcdUpdate();
		//}		
	}
	SDL_Quit();
	exit(0);
	return 0;
}

