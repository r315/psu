#include "common.h"
#include "system.h"
#include "spi.h"
#include "pwm.h"
#include "display.h"
#include "rtc.h"

typedef struct{
	char cur;
	char last;
	char events;
	unsigned int counter;	
    unsigned int htime;
}BUTTON_Controller;

#define BUTTON_DEFAULT_HOLD_TIME 2000   //2 seconds

//bank1 static _rkey;
bank1 BUTTON_Controller __button;

void systemInit(void){	
	__CONFIG(0x3FBA);
	ADCON0 = 0b11000001;  // Frc clock | ADON
	ADCON1 = 0b00000011;  // RA3 = ref, AN0,1,2,5
	OPTION &= ~0x80;      // enable pull-ups on PORTB
	TRISA = TRISA_VALUE;
	TRISB = TRISB_VALUE;
	TRISC = TRISC_VALUE;
	PORTB = 255; // 0?
	spiInit(SPI_MODE3); 
	lcdInit();	
	rtcInit();
	setFpwm(PWMFREQ);	
	setDuty(ISET_CH,MIN_IOUT_PWM_VAL);
	setDuty(VSET_CH,MIN_VOUT_PWM_VAL);
	
	__button.cur  = BUTTON_EMPTY;
    __button.last = BUTTON_EMPTY;
    __button.events = BUTTON_EMPTY;
    __button.htime = 500;
}

char done(void) { return 0;}
void finish(void){};
void enableLoad(char state){LOAD_EN = state;}
/*
//char scanKeys(void){return (~BPORT) & BPORTMask;}
char scanKeys(void){
uchar ky;
	ky = (~BPORT) & BPORTMask;
	if(ky){
		if(ky == (_rkey & (~K_HOLD) )){
			_rkey |= K_HOLD;
			return 0;
		}
	}
	_rkey = ky;
	return _rkey;
}
//char keyDown(char k){return (~BPORT) & k & BPORTMask;}
char keyDown(char k){return _rkey & k;}
char getKey(void){
	while(!scanKeys());
return _rkey;
}
*/


char scanKeys(void){
char cur = ((~BPORT) & BPORTMask);

    switch(__button.events){

        case BUTTON_EMPTY:
            if(cur == BUTTON_EMPTY)
                break;            
            __button.cur = cur;
            __button.events = BUTTON_PRESSED;
            break;

        case BUTTON_PRESSED:
            if(cur == BUTTON_EMPTY){
                __button.events = BUTTON_RELEASED;
                break;
            }
            if(cur == __button.cur){             // same key still pressed
                __button.events = BUTTON_TIMING; // start timer
                __button.counter = 0;
                break;
            }
            __button.cur = cur; // another key was pressed 
            break;              // TODO: optionaly implement if one key is relesed

        case BUTTON_TIMING:
            if(cur == BUTTON_EMPTY){
                __button.events = BUTTON_RELEASED;
                break;
            }            
            if(cur == __button.cur){
                if( (++__button.counter) == __button.htime){
                    __button.events = BUTTON_HOLD;
                }   
                break;
            }
            __button.cur = cur; // another key was pressed 
            __button.events = BUTTON_PRESSED;
            break;

        case BUTTON_HOLD:
            if(cur == BUTTON_EMPTY){
                __button.events = BUTTON_RELEASED;
            }
            if(cur == __button.cur)
                break;
            __button.cur = cur; // another key was pressed 
            __button.events = BUTTON_PRESSED;
            break;
            
        case BUTTON_RELEASED:
            __button.last= __button.cur;
            __button.cur = BUTTON_EMPTY;
            __button.events = BUTTON_EMPTY;
			delayMs(50);
            break;
            
        default: break;
    }
    return __button.cur;
}

char scanKeysState(void){	
	return __button.events;
}

char scanKeysValue(void){
	return __button.cur;
}

//--------------------------------------------
// scans keys and update inc/dec param
// if key detected
//--------------------------------------------
char scanKeysAndUpdateValue(uchar max, uchar min, uchar *var){	
	scanKeys();
	if(scanKeysState() == BUTTON_PRESSED || scanKeysState() == BUTTON_HOLD) 
		return updateValueForKey(max,min,var);
	return BUTTON_EMPTY;
}

char updateValueForKey(uchar max, uchar min, uchar *var){
	switch(scanKeysValue()){
		case L_KEY:
			if(*var > min)
				(*var)--;
			break;	
	
		case R_KEY:
			if(*var < max)
				(*var)++;
			break;	
	}
	return scanKeysValue();
}

uchar ADIN(char ch)
{	
	ADCON0 &=0xC1;		// clear current channel select	
	ADCON0 |= (ch<<3);	// apply the new channel select
	asm("nop");asm("nop");
	asm("nop");asm("nop");
	ADGO = 1;			// initiate conversion on the selected channel
	while(ADGO);
return ADRES;
}

int getVout(char ch){
	return ADIN(ch);
}

int getIout(void){
	return ADIN(IOUT_CH);
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

void disableOutput(void){
	enableLoad(1);
	setDuty(ISET_CH,MIN_IOUT_PWM_VAL);
}
