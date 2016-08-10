#include "common.h"
#include "display.h"

void printChar(char c, char atr){
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

void printText(char c, char p, const char *text){
	lcdsetPos(c,p);
	while(*text)
		printChar(*text++,NORMAL);
}

void printInt(uchar c, uchar p, unsigned int value)
{
#define radix 10
 char k, i=0, tmp[8];
 
	do{
		k = (char)(value % radix);
		if(k > 9)
			k += 'A';
		else
			k += '0';
		value /= radix;			
		tmp[i++] = k;		
	}while(value != 0);		
	
	lcdsetPos(c,p);
	while(i--)
		printChar(tmp[i],NORMAL);
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

struct MenuItem *selectMenuItem(struct MenuItem *items, char nitems, uchar spacing){
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
		lcdsetPos(i,1);		
		lcdData(0x01);		// linha H por baixo titulo
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
char drawBigNumber(char x, char y, char d)
{
char i; 
const char *e =
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

void printBigDecimal(char x, char y, unsigned int val, unsigned int div, char dot){
unsigned int dig;
	while(div){
		dig = val / div;		
		x = drawBigNumber(x,y,(dig%10));	
		if(div == dot){ 
			lcdData(0x30);	//place decimal dot
			lcdData(0x30);
			x += 4;			//position for next digit
		}
		div /= 10;
	}
}
