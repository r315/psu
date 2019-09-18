#include "seven_seg.h"
#include "lcd.h" 

#define SEVEN_DIG_W     17
#define SEVEN_DIG_H     30

#define SEG_W   9
#define SEG_H   4

#define SEG_A  cx + 4, cy + 0, segments
#define SEG_B  cx + 13, cy + 4, segments + (SEG_W * SEG_H) + 2
#define SEG_C  cx + 13, cy + 17, segments + (SEG_W * SEG_H) + 2
#define SEG_D  cx + 4, cy + 26, segments
#define SEG_E  cx + 0, cy + 17, segments + (SEG_W * SEG_H) + 2
#define SEG_F  cx + 0, cy + 4, segments + (SEG_W * SEG_H) + 2
#define SEG_G  cx + 4, cy + 13, segments
#define SEG_DP cx + 17, cy + 27, segments + (2 * ((SEG_W * SEG_H) + 2))

#define UNIT_VOLT     56,0, (uint8_t*)units
#define UNIT_AMP      123,0, (uint8_t*)(units + 6)

static uint16_t cx,cy;

static const uint8_t segments[]={
// a
    SEG_W,SEG_H,
    0,1,1,1,1,1,1,1,0,
    1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,
    0,1,1,1,1,1,1,1,0,
// b
    SEG_H,SEG_W,
    0,1,1,0,
    1,1,1,1,
    1,1,1,1,
    1,1,1,1,
    1,1,1,1,
    1,1,1,1,
    1,1,1,1,
    1,1,1,1,
    0,1,1,0,
//dp
    2,2,
    1,1,
    1,1,
 
};

static const uint8_t units[]={
    5,
    0x0f, 0x18, 0x30, 0x18, 0x0f,
    5,
    0x3E, 0x09, 0x09, 0x09, 0x3E
};


/**
 * Draws a single segment
 * \param x,y - Digit origin
 * \param seg - Segment (a,b,c,d,e,f,g,dp)
 * */
void drawSegment(uint16_t x, uint16_t y, const uint8_t *segData){
uint8_t w,h;

    w = *segData++;
    h = *segData++;

    for(uint8_t j = 0; j < h; j++){
        for(uint8_t i = 0; i < w; i++, *segData++){
            if(*segData){
                LCD_Pixel(x + i, y + j, WHITE);
            }
        }
    }
}

void SEVEN_Dp(void){
    drawSegment(SEG_DP);
}


void SEVEN_Dig(uint8_t dig){
    LCD_ClrArea(cx, cy, SEVEN_DIG_W, SEVEN_DIG_H);
    switch(dig){
        case 0:
            drawSegment(SEG_A);
            drawSegment(SEG_B);
            drawSegment(SEG_C);
            drawSegment(SEG_D);
            drawSegment(SEG_E);
            drawSegment(SEG_F);
            break;

        case 1:
            drawSegment(SEG_B);
            drawSegment(SEG_C);            
            break;
        case 2:
            drawSegment(SEG_A);
            drawSegment(SEG_B);
            drawSegment(SEG_D);
            drawSegment(SEG_E);            
            drawSegment(SEG_G);
            break;
        case 3:
            drawSegment(SEG_A);
            drawSegment(SEG_B);
            drawSegment(SEG_C);
            drawSegment(SEG_D);            
            drawSegment(SEG_G);
            break;
        case 4:
            drawSegment(SEG_B);
            drawSegment(SEG_C);
            drawSegment(SEG_F);
            drawSegment(SEG_G);            
            break;
        case 5:
            drawSegment(SEG_A);
            drawSegment(SEG_C);
            drawSegment(SEG_D);
            drawSegment(SEG_F);
            drawSegment(SEG_G);            
            break;
        case 6:
            drawSegment(SEG_A);
            drawSegment(SEG_G);
            drawSegment(SEG_C);
            drawSegment(SEG_D);
            drawSegment(SEG_E);
            drawSegment(SEG_F);
            break;
        case 7:
            drawSegment(SEG_A);
            drawSegment(SEG_B);
            drawSegment(SEG_C);           
            break;
        case 8:
            drawSegment(SEG_A);
            drawSegment(SEG_B);
            drawSegment(SEG_C);
            drawSegment(SEG_D);
            drawSegment(SEG_E);
            drawSegment(SEG_F);
            drawSegment(SEG_G);
            break;
        case 9:
            drawSegment(SEG_A);
            drawSegment(SEG_B);
            drawSegment(SEG_C);
            drawSegment(SEG_D);
            drawSegment(SEG_G);
            drawSegment(SEG_F);
            break;  

        default:  
            drawSegment(SEG_G);
            break;
     
    }
    cx += SEVEN_DIG_W + 2;
}

void xitoa (long val, signed char radix, signed char len){
	unsigned char c, r, sgn = 0, pad = ' ';
	unsigned char s[16], i = 0;
	unsigned int v;

	if (radix < 0) {
		radix = -radix;
		if (val < 0) {
			val = -val;
			sgn = '-';
		}
	}
	
	v = val;
	r = radix;
	
	if (len < 0) {
		len = -len;
		//pad = '0';
        pad = 0;
	}
	
	if (len > 16) len = 16;
	
	do {
		c = (unsigned char)(v % r);
		if (c >= 10) c += 7;
		//c += '0';
		s[i++] = c;
		v /= r;
	} while (v);
	
	if(sgn) 
		s[i++] = sgn;
	
	while (i < len)
		s[i++] = pad;
		
	do{
		SEVEN_Dig(s[--i]);
	}while (i);
}

void SEVEN_Double(uint16_t x, uint16_t y, double val){
long int_part, frac_part;
char prec, places = 2;
  
    int_part = (long)(val);
		
	frac_part = 0;
	prec = 0;
	while ((prec++) < places){
        val *= 10;
        frac_part = (frac_part * 10) + (long)val - ((long)val / 10) * 10;  // + ((long)f%10);			
	}

    cx = x;
    cy = y;

    SEVEN_Dp();
    SEVEN_Dig(int_part);
    xitoa((frac_part < 0)? -frac_part : frac_part, 10, -2);  
}

void SEVEN_Init(void){
    LCD_Init();
    LCD_SetFrame(UNIT_VOLT);
    LCD_SetFrame(UNIT_AMP);
}

void SEVEN_PrintAmps(double amps){
    SEVEN_Double(0,1,amps);
}

void SEVEN_PrintVolts(double volts){
    SEVEN_Double(65,1,volts);
}