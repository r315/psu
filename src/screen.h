#ifndef _screen_h_
#define _screen_h_

/* screen 1.5 */


#define M1 1//6
#define M2 48
#define M3 84
#define ML 7

#define LFRAMESIZ 80
#define NORMAL 0
#define INVERTED 255
#define UNDERLINE 128

#define SET_CURRENT_C 88
#define SET_CURRENT_P 3
#define SET_VOLTAGE_C 88
#define SET_VOLTAGE_P 2

#define CURRENT_C 10  // posição dos digitos grandes
#define CURRENT_P 4

#define VOLTAGE_C 17  // posição dos digitos grandes
#define VOLTAGE_P 2

#define WATTAGE_C 88  
#define WATTAGE_P 5
#define WATT_C1 69
#define WATT_P1 2

#define GRAPH_C (LCD_W-62)
#define GRAPH_P 6
#define GRAPHSIZE 60


#define VMENU 0
#define AMENU 1
#define LMENU 2

#define setFontPatern(p) _inv = p

extern char _inv,_outstr[10];
void drawFrame(void);
void printVoltage(unsigned int v);
void printCurrent(unsigned int a);
void printWattage(unsigned int v, unsigned int a);

void printString(char c, char p, const char *str);
void printInt(char c, char p, unsigned int v, char radix);
char printIntBig(char x, char y, const char *v);
#endif
