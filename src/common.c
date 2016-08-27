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
	clrMenu();
	printTextAtr(MENU_START_COL, MENU_PAGE, mitem->name, INVERTED);	
}

//------------------------------------------
//draw all items that compose menu
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
		if(done)
			drawMenuItems(items,nitems,selection);
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
	printTextAtr(col, 0, title, INVERTED);
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

void drawSetIcon(uchar c1, uchar p1){
	uchar i;
	lcdsetPos(c1,p1);
	for(i = 0; i < sizeof(bmSET); i++){
		lcdData(~bmSET[i]);
	}	
}

void clrSetIcon(uchar c1, uchar p1){
	uchar i;
	lcdsetPos(c1,p1);
	for(i = 0; i < sizeof(bmSET); i++){
		lcdData(0);
	}		
}

