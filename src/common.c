#include "common.h"
#include "display.h"
#include "font.h"

void printChar(uchar c, uchar atr){
	c -= 0x20;
	lcdData(FONT[c][0]^atr);
	lcdData(FONT[c][1]^atr);
	lcdData(FONT[c][2]^atr);
	lcdData(FONT[c][3]^atr);
	lcdData(FONT[c][4]^atr);
	lcdData(0^atr);
}

void printString(string *str){
	const char *p = str->chars;
	lcdsetPos(str->col,str->page);
	while (*p) 
		printChar(*p++,str->atribute);
}

void printText(uchar c, uchar p, const char *text){
	lcdsetPos(c,p);
	while(*text)
		printChar(*text++,NORMAL);
}

void drawMenu(string *mns){
#define MENU_PAGE 7	
	mns->page = MENU_PAGE;
	mns->atribute |= UNDERLINE;
	printString(mns);
	lcdUpdate();
}

void clrMenu(void){
uchar i;
	lcdsetPos(1,MENU_PAGE);
	for(i=1; i<LCD_W-1;i++){
		lcdData(UNDERLINE);
	}
}

void clrCanvas(void){
uchar i,j;	
	for(j = 1; j < 7;j++){
		lcdsetPos(1,j);
		for(i=1; i<LCD_W-1;i++)
			lcdData(j == 6 ? UNDERLINE : NORMAL);
	}
}

void drawMenuItem(menuitem *mitem){
string item;
	clrMenu();
	item.chars = mitem->name;
	item.atribute = INVERTED;
	item.col = MENU_START;
	item.page = MENU_PAGE;
	printString(&item);
}

void drawMenuItems(struct MenuItem *items, uchar nitems, uchar highlight, uchar spacing){
uchar i;
string item;
	clrMenu();
	for(i = 0; i < nitems; i++){
		item.chars = items[i].name;
		item.atribute = i == highlight ? INVERTED:UNDERLINE;
		item.col = MENU_START + (i*spacing);
		drawMenu(&item);		
	}
	lcdUpdate();
}

struct MenuItem *selectMenuItem(struct MenuItem *items, uchar nitems, uchar spacing){
unsigned char done = 1, selection = 0;	
	do{
		if(done)
			drawMenuItems(items,nitems,selection,spacing);
		done = readKeysUpdate(nitems-1,0,&selection);			
	}while (done != M_KEY);	
	return &items[selection];
}
//------------------------------------------------------
//      -------------------------
//      |        titulo         |
//      -------------------------
//      |                       |
//      |                       |
//      |                       |
//      -------------------------
//      |    menu               |
//      -------------------------
//------------------------------------------------------
void drawFrame(string *title){
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
	printString(title);
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

void printVoltage(uchar c, uchar p, unsigned int mv)
{
	mv = mv / 100;
	printDecimal(c,p,BIG_DIGIT,mv,100,10);
}

void printCurrent(uchar c, uchar p, unsigned int ma)
{
	printDecimal(c,p,BIG_DIGIT,ma,1000,0);	
}

void drawSetIcon(uchar c, uchar p){
	uchar i;
	lcdsetPos(c,p);
	for(i = 0; i < sizeof(bmSET); i++){
		lcdData(~bmSET[i]);
	}	
}

