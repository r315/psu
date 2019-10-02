
#ifdef WIN32
    #include "ks0713.h"
#else
    #include "lcd.h"	
#endif

#include "screen.h"
#include "font.h"

char _outstr[10], _inv;
char drawBigDigit(char x, char y, char d);
void intToString(char* dst, unsigned int value, char radix);

#ifdef WIN32
char graphdata[GRAPHSIZE],ndata;
#else
bank1 char graphdata[GRAPHSIZE],ndata;
#endif
//------------------------------------------------------
//print voltage
//------------------------------------------------------
void printVoltage(unsigned int v)
{
unsigned char y = VOLTAGE_P, x = VOLTAGE_C;
int dig = v/1000;	
	x = drawBigDigit(x,y,(dig/10));
	x = drawBigDigit(x,y,(dig%10));
	lcddata(0x30);
	lcddata(0x30);
	x+=4;
	dig = v%1000;
	if(dig>100)
		x = drawBigDigit(x,y,(dig/100));
	else
		x = drawBigDigit(x,y,0);	
}
//------------------------------------------------------
//print current
//------------------------------------------------------
void printCurrent(unsigned int c)
{
unsigned char y = CURRENT_P, x = CURRENT_C,i;
unsigned int dig, div = 1000;
	for(i=0;i<4;i++)
	{
		dig = c / div;
		x = drawBigDigit(x,y,(dig%10));
		div /= 10;
	}
}
//------------------------------------------------------
//print normal char with background pattern
//------------------------------------------------------
void printc(char c)
{
	c -= 0x20;
	lcddata(FONT[c][0]^_inv);
	lcddata(FONT[c][1]^_inv);
	lcddata(FONT[c][2]^_inv);
	lcddata(FONT[c][3]^_inv);
	lcddata(FONT[c][4]^_inv);
	lcddata(0^_inv);
}
//------------------------------------------------------
//print string with background pattern
//------------------------------------------------------
void printString(char c, char p, const char *str)
{
	lcdsetPos(c,p);
	while (*str) 
		printc(*str++);			
}
//------------------------------------------------------
//
//------------------------------------------------------
void printInt(char c, char p, unsigned int v, char radix)
{
	intToString(_outstr,v,radix);	
	printString(c,p,_outstr);
}
//------------------------------------------------------
//print decimal value
//------------------------------------------------------
void printDval(char c, char p, unsigned int v)
{
unsigned int unidade = v/1000;
char tmp[3];
	
	lcdsetPos(c,p);
	intToString(tmp,unidade,10);	
	
	printc(tmp[0]);
	
	if(unidade>9)       //if integer part is two digit, print second
		printc(tmp[1]);
		
	printc('.');
	
	unidade = v%1000;	
	
	if(unidade>100)
		printc((unidade/100)+'0');
	else
		printc('0');
}
//------------------------------------------------------
//draw number 0-9 in big font
//------------------------------------------------------
char drawBigDigit(char x, char y, char d)
{
char i; 
const char *e;

	e = HITACHI_FONT + (d * HITACHI_FONT_LINE_BYTES);	
	// upper half
	lcdsetPos(x,y);
	for(i = 0; i< HIT_FONT_W; i++)
		lcddata(*e++);
	lcddata(0);

	// lower half	
	lcdsetPos(x,y+1);
	for(i = 0; i<HIT_FONT_W; i++)
		lcddata(*e++);
	lcddata(0);
return x + HIT_FONT_W + 1;
}
//------------------------------------------------------
//draw int in big font
//------------------------------------------------------
char printIntBig(char x, char y, const char *v)
{
    while(*v)
	x = drawBigDigit(x, y, (*v++));
return x;		
}
//------------------------------------------------------
//      -------------------------
//      |            |          |
//      |            |          |
//      |            |          |
//      -------------------------
//------------------------------------------------------
void loadFrame(void)
{
unsigned char i;
	
	drawFrame();
	
	for(i=0; i<7 ;i++){
		lcdsetPos(GRAPH_C,i);// linha V separadora de frames
		lcddata(255);
	}
	setFontPatern(NORMAL);				
}
//------------------------------------------------------
//      -------------------------
//      |            |set       |
//      |            |          |
//      |            |          |
//      |            -----------|
//      |                       |
//      -------------------------
//------------------------------------------------------
const char bmSET[]={174,170,186,128,190,170,162,128,130,190,130,128};
void psFrame(void)
{
unsigned char i;
	drawFrame();
	for(i=0; i<4 ;i++){
		lcdsetPos(LFRAMESIZ,i);// linha V separadora de frames
		lcddata(255);
	}
	lcdsetPos(LFRAMESIZ,i);
	lcddata(0x1f);
	for(i=LFRAMESIZ+1; i< LCD_W-1 ;i++){// linha H separadora de frames
		lcddata(0x10);
	}
	
	
	lcdsetPos(LFRAMESIZ+1,1);  // bitmap SET
	for(i = 0; i < sizeof(bmSET); i++){
		lcddata(~bmSET[i]);
	}	
	
	setFontPatern(NORMAL);
	// unidades na janela set
	printString(SET_VOLTAGE_C+19,VOLTAGE_P,"V");
	printString(SET_CURRENT_C+19,CURRENT_P-1,"A");
	printString(SET_CURRENT_C+19,WATTAGE_P,"W");

	
}
//------------------------------------------------------
//
//      -------------------------
//      |   titulo              |
//      -------------------------
//      |                       |
//      |                       |
//      |                       |
//      -------------------------
//      |    menu               |
//      -------------------------
//
//------------------------------------------------------
void drawFrame(void)
{
unsigned char i,j;

	for(i=0;i<8;i++)   //limpa o lcd
	{
		lcdsetPos(0,i);
		for(j=0;j<LCD_W;j++)
			lcddata(0x0);
	}
	
	lcdsetPos(0,0);
	for(i = 0; i<LCD_W;i++) // barra titulo
		lcddata(INVERTED);	
	
	for(i=0;i<LCD_W;i++){
		lcdsetPos(i,1);		
		lcddata(0x01);		// linha H por baixo titulo
		lcdsetPos(i,6);	
		lcddata(0x80);		// linha H topo menu
		lcdsetPos(i,7);	
		lcddata(0x80);		// linha H fundo menu
	}
	
	for(i=1;i<LCD_H/8;i++){
		lcdsetPos(0,i);
		lcddata(0xFF);		// linhas laterais	
		lcdsetPos(LCD_W-1,i);
		lcddata(0xFF);
	}	
	// unidades na janela principal comuns a todos os menus
	setFontPatern(NORMAL);
	printString(54,VOLTAGE_P,"V");
	printString(54,CURRENT_P,"mA");	
}
//------------------------------------------------------
//
//------------------------------------------------------
void updateGraph(void)
{
unsigned char i,tmp,pat,offset;

	if(ndata & 0x80)
		offset = ndata & 0x7F;
	else
		offset = 0;
		
	for(i = 0; i < GRAPHSIZE; i++,offset++)
	{
		lcdsetPos(GRAPH_C+i+1,GRAPH_P);  //limpa o traco antigo
		lcddata(0x80);
		lcdsetPos(GRAPH_C+i+1,GRAPH_P-1);// tem de ser limpo nas duas paginas
		lcddata(0);
		tmp = graphdata[offset%GRAPHSIZE];
		pat = 0x80 >> (tmp&7);
		if(tmp<8) pat |= 0x80; // para corrigir a sobreposição da linha superior do menu
		lcdsetPos(GRAPH_C+i+1,GRAPH_P-(tmp>>3));
		lcddata(pat);
	}		
}
void addgraphData(char data){
	graphdata[ndata&0x7F] = data;
	ndata++;
	if((ndata&0x7F) == GRAPHSIZE)
		ndata = 0x80; // aciva a flag a indicar que chegou ao estremo do grafico						  
}
	
