#include "common.h"
#include "display.h"
#include "font.h"

bank1 mesure outvalues; //TODO: min ensure current when change modes(PSU/load)

void printCostumChar(uchar c){
uchar i;
const char *p;
	c -= 0x80;
	p = &special_chars[c][0];
	for(i=0; i< 6; i++)
		lcdData(*p++);
}

void printChar(uchar c, uchar atr){
	/*if(c > 127){
		printCostumChar(c);
		return;
	}*/
	c -= 0x20;
	lcdData(FONT[c][0]^atr);
	lcdData(FONT[c][1]^atr);
	lcdData(FONT[c][2]^atr);
	lcdData(FONT[c][3]^atr);
	lcdData(FONT[c][4]^atr);
	lcdData(0^atr);
}

uchar printTextAtr(uchar c, uchar p, const char *text, uchar atr){
uchar x = 0;
	lcdsetPos(c,p);
	while(*text){
		printChar(*text++,atr);
		x++;
	}
	return c + (x*FONT_W);
}

void printText(uchar c, uchar p, const char *text){
	printTextAtr(c,p,text,NORMAL);
}

void clrMenu(void){
uchar i;
	lcdsetPos(1,MENU_PAGE);
	for(i = 1; i < LCD_W-1; i++){
		lcdData(UNDERLINE);
	}
}

void clrSetsArea(void){
uchar i,j;	
	for(j = 1; j < 7;j++){
		lcdsetPos(DRO_SIZE+1,j);
		for(i = DRO_SIZE+1; i < LCD_W-1; i++)
			lcdData(j == 6 ? UNDERLINE : NORMAL);
	}
}

//------------------------------------------
//draw single menu item 
//------------------------------------------
void drawMenuItem(struct MenuItem *mitem){
	clrMenu();
	printTextAtr(MENU_START_COL, MENU_PAGE, mitem->name, INVERTED);	
	lcdUpdate();
}

//------------------------------------------
//draw all items that compose menu
//and highlight the selected
//------------------------------------------
void drawMenuItems(struct MenuItem *items, uchar nitems, uchar highlight){
uchar i,x = MENU_START_COL;
	clrMenu();
	for(i = 0; i < nitems; i++){
		x = printTextAtr(x, MENU_PAGE, items[i].name, (i == highlight) ? INVERTED:UNDERLINE);			
	}
	lcdUpdate();
}

struct MenuItem *selectMenuItem(struct MenuItem *items, uchar nitems){
unsigned char done = 1, selection = 0;	
	do{
		scanKeys();
		if(scanKeysState() == BUTTON_PRESSED){
		//if(done)
			//drawMenuItems(items,nitems,selection);
			drawMenuItem(&items[selection]);
		//done = scanKeysAndUpdateValue(nitems-1, 0, &selection);
		done = updateValueForKey(nitems-1, 0, &selection);
		}else
			done = BUTTON_EMPTY;
	}while (done != M_KEY);	
	
	return &items[selection];
}
//------------------------------------------------------
//      -------------------------
//      |        titulo         |
//      -------------------------
//      |               |     V |
//      |               |     mA|
//      |               |     W |
//      |               |       |
//      -------------------------
//      |    menu               |
//      -------------------------
//------------------------------------------------------

void drawFrame(const char *title, uchar col){
unsigned char i;

	solidFill(NORMAL);
	
	lcdsetPos(0,0);
	for(i = 0; i < LCD_W;i++)// barra titulo
		lcdData(INVERTED);	
	
	for(i=0;i<LCD_W;i++){		
		lcdsetPos(i,6);	
		lcdData(0x80);		// linha H topo menu
		lcdsetPos(i,7);	
		lcdData(0x80);		// linha H fundo menu
	}
	
	for(i=1;i<LCD_H/8;i++){
		lcdsetPos(0,i);
		lcdData(0xFF);		// linhas laterais	
		lcdsetPos(LCD_W-1,i);
		lcdData(0xFF);
	}	

	for(i=1; i < 7 ;i++){
		lcdsetPos(DRO_SIZE,i);// linha V separadora de frames
		lcdData(255);
	}

	printText(48,2,"V");
	printText(54,4,"mA");	
	printTextAtr(col, 0, title, INVERTED);
	drawSetsUnits();
}

void drawSetsUnits(void){
	printText(114,2,"V");
	printText(114,3,"mA");
	printText(114,4,"W");
}
//------------------------------------------------------
//draw number 0-9 in big font
//------------------------------------------------------
char drawBigNumber(uchar x, uchar y, uchar d)
{
char i; 
const char *e;
	
	if(d == '.'){
		lcdData(0);		//separate dot from digit
		lcdData(0x30);	//place decimal dot
		lcdData(0x30);
		return x + 3;
	}

	d -= '0';
	e = HITACHI_FONT + (d * HITACHI_FONT_LINE_BYTES);	
	// upper half
	lcdsetPos(x,y);
	for(i = 0; i< HIT_FONT_W; i++)
		lcdData(*e++);

	// lower half	
	lcdsetPos(x,y+1);
	for(i = 0; i<HIT_FONT_W; i++)
		lcdData(*e++);

return x + HIT_FONT_W + 1;
}

char callprintChar(uchar x, uchar y, uchar d){
	printChar(d,NORMAL);
	return x + FONT_W + 1;
}

void printDecimal(uchar x, uchar y, uchar bigdigit, unsigned int val, unsigned int magnitude, uchar dot){
unsigned int dig;
char (*drawDigit)(uchar x, uchar y, uchar d);

	if(bigdigit){
		drawDigit = drawBigNumber;
	}else{
		lcdsetPos(x,y);
		drawDigit = callprintChar;
	}

	while(magnitude){
		dig = val / magnitude;		
			x = drawDigit(x,y,(dig%10) + '0');	
			if(magnitude == dot){
				x = drawDigit(x,y,'.');	
		}		
		magnitude /= 10;
	}
}

void drawSetIcon(uchar c1, uchar p1){
	lcdsetPos(c1,p1);
	printCostumChar(CHAR_SET_LEFT);
	printCostumChar(CHAR_SET_RIGHT);
}

void clrSetIcon(uchar c1, uchar p1){
	lcdsetPos(c1,p1);
	printChar(' ',NORMAL);
	printChar(' ',NORMAL);
}

void updateDro(){
	getMesures(&outvalues);	
	printDecimal(DRO_VOLTAGE_COL,DRO_VOLTAGE_PAGE,BIG_DIGIT, outvalues.ch1_voltage/100 ,100,10);
	printDecimal(DRO_CURRENT_COL, DRO_CURRENT_PAGE, BIG_DIGIT, outvalues.current, 1000, 0);
	printDecimal(DRO_POWER_COL, DRO_POWER_PAGE, NORMAL_DIGIT, outvalues.power, 100, 10);
}
