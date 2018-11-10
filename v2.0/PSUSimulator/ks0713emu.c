
//Lib SDL 1.2
#include "ks0713emu.h"


SDL_Surface* screen ;
Uint32 _pixel_pointer;
uint16_t _page,_column;

#define OFFPIXEL 0x2E9118
#define ONPIXEL 0
#define SCALE 4

void drawScaledPixel(char y, Uint32 shade){
Uint32 *pixels = (Uint32 *)screen->pixels;
int k,n;
	for(k = 0; k < SCALE; k++)
		for(n = 0; n < SCALE; n++)
			pixels[(_page*LCD_PAGES*SCALE*LCD_W*SCALE)+(LCD_W*SCALE*k)+(y*SCALE*LCD_W*SCALE) + _column + n] = shade;			
}

//------------------------------------------------------
//
//------------------------------------------------------
void lcdData(unsigned char data){ 
 Uint32 shade;
 char i;  
   if(_column < LCD_W*SCALE && _page < LCD_PAGES*SCALE)
   { 
		for(i = 0; i < 8; i++) //data bits are drawn verticaly
		{
			if(data &(1<<i))
				shade = ONPIXEL;
			else
				shade = OFFPIXEL;
			drawScaledPixel(i,shade);			
		}        
      _column += SCALE; 
    }   	
}
//------------------------------------------------------
//
//------------------------------------------------------
void lcdsetPos(unsigned char x, unsigned char y)
{
	_column = (x*SCALE);
	_page = y & 7;	
}
//------------------------------------------------------
//
//------------------------------------------------------
void solidFill(char d)
{
	unsigned char p,c;
	for(p = 0; p < LCD_PAGES; p++){
		lcdsetPos(0,p);
		for(c=0;c<LCD_W;c++)
			lcdData(d);
	}
}
//------------------------------------------------------
//
//------------------------------------------------------
void lcdInit(void)
{
 	screen = NULL;
    SDL_Init( SDL_INIT_EVERYTHING );    
    screen = SDL_SetVideoMode(LCD_W*SCALE,LCD_H*SCALE,32, SDL_SWSURFACE );
	SDL_WM_SetCaption("ks0713", NULL);	
	_page=_column=0;
	solidFill(0x00);	
}

void lcdUpdate(void){
	SDL_Flip(screen);
}



