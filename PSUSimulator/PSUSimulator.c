
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "system.h"
#include "selector.h"

#define TITLE "PIC PSU V2.0"
string title = { TITLE,sizeof(TITLE),INVERTED,(LCD_W/2)-(sizeof(TITLE)*(FONT_W/2)),0 };

void powerSupply(void);
void electronicLoad(void);

struct MenuItem modes[]={
	{" PSU ",powerSupply},		//power Supply	
	{" Lod ",electronicLoad},	//constant load
	{" Bat ",powerSupply},		//battery discharge curve an capacity
	{" OPT ",powerSupply}		//
};

//------------------------------
//
//------------------------------
int main(int argc, char *argv[])
{
	systemInit();
	drawFrame(&title);
	lcdUpdate();
	while (!done()){		
		  ((void(*)(void))(changeMode(modes,sizeof(modes)/sizeof(struct MenuItem))->data))();
		  clrCanvas();
	}
	SDL_Quit();
	exit(0);
	return 0;
}

