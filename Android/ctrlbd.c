/*******************************************************************
psu control board firmware
05/14	
********************************************************************/
#include "system.c"
#include <pwm.c>
#include <delay.c>
  
enum {
	WRITE_DATA_H,
	READ_DATA_H,
	WRITE_DATA_L,
	READ_DATA_L,
	IDLE
};

// dataH                       datal
// |F2|F1|F0|RD|S1|S0|D9|D8|   |d7|d6|d5|d4|d3|d2|d1|d0|	
struct dataPack{
	char datal;
	char datah;	
	char status;	
}i2cTransfer;

void (*functions[])(int *) = {dummyFunction,getFunctions,
							  setAdc,getAdc,
							  setPwm,getPwm,
							  setGpo,getGpi};
void (*doFunc)(int *);
//---------------------------------------------------
//TODO: handle errors
//---------------------------------------------------
void interrupt handler(void){
	
	if(SSPIF){		
		if(!DA){
			if(RW){		
				doFunc((int*)&i2cTransfer.datal);
				SSPBUF = i2cTransfer.datah;
				SSPCON |= SCL_ENABLE;
				i2cTransfer.status = READ_DATA_L;
			}else{
				i2cTransfer.status = SSPBUF; // clear BF
				i2cTransfer.status = WRITE_DATA_H;
			}
		}else{			
			switch(i2cTransfer.status){
				case WRITE_DATA_H:						
						i2cTransfer.datah = SSPBUF;
						i2cTransfer.status = WRITE_DATA_L;
						break;						
				case WRITE_DATA_L:
					i2cTransfer.datal = SSPBUF;
					doFunc = functions[i2cTransfer.datah >> 4];					
					i2cTransfer.status = IDLE;
					doFunc((int*)&i2cTransfer.datal);
					break;						 
				case READ_DATA_L:
					SSPBUF = i2cTransfer.datal;
					SSPCON |= SCL_ENABLE;
					i2cTransfer.status = IDLE;
					break;		
				default:
					i2cTransfer.status = IDLE;
			}
		}
		if(WCOL || SSPOV){
			WCOL = 0;
			SSPOV = 0;			
			i2cTransfer.status = SSPBUF; // clear BF
			i2cTransfer.status = IDLE;		
		}			  
		SSPIF = 0;	
	}
}
//---------------------------------------------------
//
//---------------------------------------------------
void main(void)
{
char a;
	systemInit();
	i2cSetup();  
	i2cTransfer.status = IDLE;
	pwmSetup();	
	doFunc = functions[0];

	while(1){
		
	}	
}

        
