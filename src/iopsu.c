
#include "system.h"
#include "spi.h"
#include "pwm.h"
#include "display.h"
#include "rtc.h"

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
}

char done(void) { return 0;}
void finish(void){};
void enableLoad(char state){LOAD_EN = state;}

bank1 static _rkey;
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

//--------------------------------------------
// scans keys and update inc/dec param
// if key detected
//--------------------------------------------
char scanKeysAndUpdateValue(uchar max, uchar min, uchar *var){	
	if(!scanKeys()) 
		return 0;	
	return updateValueForKey(max,min,var);
}

char updateValueForKey(uchar max, uchar min, uchar *var){

	switch(_rkey){
		case L_KEY:
			if(*var > min)
				(*var)--;
			break;	
	
		case R_KEY:
			if(*var < max)
				(*var)++;
			break;	
	}
	return _rkey;
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
