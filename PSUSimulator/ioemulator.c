
//Lib SDL 1.2
#include "system.h"
#include "display.h"

static SDL_Event event;
Uint8* inkeys;
//----------------------------------------------------*/
//
//------------------------------------------------------
char SDL_scanKeys(void){	
	if(SDL_PollEvent(&event)){
		if(event.type == SDL_KEYDOWN){
   			inkeys  = SDL_GetKeyState(NULL);
			return 1;
		}
	}
	return 0;
}

char scanKeys(void){	
	return SDL_scanKeys();
}

char keyDown(char key){	
	if(!inkeys)return 0;
	return inkeys[key];	
}

char done(void){	
	if(SDL_scanKeys()){
		if(event.type == SDL_QUIT || keyDown(SDLK_ESCAPE))
			return 1;
		return 0;
	}
	inkeys = 0;
	return 0;
}

char getKey(void){
	if(!scanKeys()) 
		return 0;

	if(keyDown(L_KEY)){		
		return L_KEY;		
	}
	
	if(keyDown(R_KEY)){		
		return R_KEY;	
	}
	
	if(keyDown(M_KEY)){		
		return M_KEY;	
	}
	return 0;
}

//------------------------------
//
//------------------------------
int getVout(char ch)
{	
	return ch ? 0:50;
}
//--------------------------------------------
// returns I in mA
//--------------------------------------------
int getIout(void)
{	
	return 100;
}

char readKeysUpdate(uchar max, uchar min, uchar *var){
	
	if(!scanKeys()) 
		return 0;

	if(keyDown(L_KEY)){
		if(*var > min)
			(*var)--;
		return L_KEY;		
	}
	
	if(keyDown(R_KEY)){
		if(*var < max)
			(*var)++;
		return R_KEY;	
	}
	
	if(keyDown(M_KEY)){		
		return M_KEY;	
	}
	return 0;
}


void delayMs(char ms){
	SDL_Delay(ms);
}

void setDuty(char ch, int duty){

}

void enableLoad(char state){}

void systemInit(void){
	lcdInit();
}
void finish(void){
	SDL_Quit();
}

void getMesures(mesure *msr){
	msr->adc_ch1 = getVout(VOUT_CH1);
	msr->adc_ch2 = 0; //getVout(VOUT_CH2); change when channel is connected
	msr->ch1_voltage = (msr->adc_ch1 - msr->adc_ch2) * VCONST; //mV	
	msr->current = getIout() * ICONST; //mA
	msr->power = (msr->ch1_voltage/100);
	msr->power *= (msr->current/100);
	msr->power /= 10;
}
