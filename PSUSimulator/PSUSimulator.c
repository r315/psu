
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "system.h"
#include "selector.h"

const char title[] =  {"PIC PSU V2.0"};

void powerSupply(void);
void electronicLoad(void);

struct MenuItem modes[]={
	{" PSU ",powerSupply},		//power Supply	
	{" Load ",electronicLoad},	//constant load
	//{" Bat ",powerSupply},		//battery discharge curve an capacity
	//{" OPT ",powerSupply}		//
};

//------------------------------
//
//------------------------------
int main(int argc, char *argv[])
{
	systemInit();
	disableOutput();
	drawFrame(title, (LCD_W/2)-(sizeof(title)*(FONT_W/2)));
	updateDro();
	lcdUpdate();
	while (!done()){		
		  ((void(*)(void))(changeMode(modes,sizeof(modes)/sizeof(struct MenuItem))->data))();
		  disableOutput();		  	  
	}
	SDL_Quit();
	exit(0);
	return 0;
}

