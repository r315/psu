#include "common.h"

void psFrame(void);
void printVoltage(unsigned int v);
void printCurrent(unsigned int c);

#define MINVOUT 22
#define MAXVOUT 255
#define MINIOUT 0
#define MAXIOUT 255

static struct MenuItem psmenu[] = {
	{"Voltage",0,0},
	{"Current",0,0},
	{"Exit",0,(void(*)(void))1}
};

#define VOLTAGE_MENU_SPACING (FONT_W*8)

void powerSupply(void){
unsigned char updateTime = 0, done = M_KEY;	
struct MenuItem *item;

	psFrame();	
	do{
		if(!updateTime){
			printVoltage(psmenu[0].var*50);
			printCurrent(psmenu[1].var);
			lcdUpdate();
			updateTime = 20;
		}
		updateTime--;

		if(done){
			if(done == M_KEY)
				item = selectMenuItem(psmenu,3,VOLTAGE_MENU_SPACING);			
			setDuty(ISET_CH,item->var);
			updateTime = 0;
		}	

		done = readKeysUpdate(MAXVOUT,MINVOUT,&item->var);		
		delayMs(20);

	}while(!item->run);
}
//------------------------------------------------------
//print voltage
//------------------------------------------------------
void printVoltage(unsigned int mv)
{
#define VOLTAGE_COL 10
#define VOLTAGE_PAGE 2
	mv = mv / 100;
	printBigDecimal(VOLTAGE_COL,VOLTAGE_PAGE,mv,100,10);
}
//------------------------------------------------------
//print current
//------------------------------------------------------
void printCurrent(unsigned int ma)
{
#define CURRENT_COL 10
#define CURRENT_PAGE 4
	printBigDecimal(CURRENT_COL,CURRENT_PAGE,ma,1000,0);	
}

//------------------------------------------------------
//      -------------------------
//      |            |set       |
//      |            |          |
//      |            |          |
//      |            +----------|
//      |            |          |
//      -------------------------
//------------------------------------------------------
static const char bmSET[]={174,170,186,128,190,170,162,128,130,190,130,128};
void psFrame(void)
{
#define LFRAMESIZ 80
unsigned char i;
	
	for(i=1; i < 7 ;i++){
		lcdsetPos(LFRAMESIZ,i);// linha V separadora de frames
		lcdData(255);
	}

	lcdsetPos(LFRAMESIZ+1,4);	
	for(i=LFRAMESIZ+1; i< LCD_W-1 ;i++){// linha H separadora de frames
		lcdData(0x10);
	}	
	
	lcdsetPos(LFRAMESIZ+1,1);  // bitmap SET
	for(i = 0; i < sizeof(bmSET); i++){
		lcdData(~bmSET[i]);
	}	
	printText(48,2,"V");
	printText(54,4,"mA");	
}

