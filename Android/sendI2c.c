
#define BPORT   PORTB 
#define BPORTMask (0x07<<3)
#define L_KEY (1<<5)
#define R_KEY (1<<3)
#define M_KEY (1<<4)

#define SCL RB1
#define SDA RB0
#define SDA0 TRISB0 = 0;
#define SDA1 TRISB0 = 1;
#define SCL0 TRISB1 = 0;
#define SCL1 TRISB1 = 1; 

#define LCDSDI RC5
#define LCDCLK RC3
#define LCDRS  RC4 
#define LCDRST RC6	

#include <picboard.c>
#include <spi.c>
#include <delay.h>
#include <ks0713.c>
#include <i2c.c>
#include <lcdio.c>

#define I2C_SLAVE (0x63 << 1)

char writeI2c(int data){
char errors = 0;
	
	i2cStart();
	i2cWrite(I2C_SLAVE);
	if(i2cgetAck())
		errors=1;
	i2cWrite(data>>8);
	if(i2cgetAck())
		errors+=2;
	i2cWrite(data);
	if(i2cgetAck())
		errors+=4;
	i2cStop();
	
	return errors;
} 

int readI2c(void){
char errors = 0;
int data;
	
	i2cStart();
	i2cWrite(I2C_SLAVE | 1);
	if(i2cgetAck())
		errors++; 
	data = i2cRead()<<8;	
	i2csendAck(0);
	data |= i2cRead();
	i2cgetAck();//i2csendAck(0);
	i2cStop();	 
	return data;
}



void main(void){

unsigned int n;

	ADCON1 = 0b00000110;  // Digital IO only
	OPTION &= ~0x80;      // enable pull-ups for keys
	
	TRISA = 255;
	TRISB = 0b11111100;
	TRISC = 0b10000111;
	PORTB = 0b11111100;
	i2cStop(); 

	spiInit();
	lcdInit();

	printString(0,0,"I2C byte sender");	
 
	while(1){
		if(keydown(L_KEY)){
			n = writeI2c(0x255);
			printString(0,2,"Error");
			printInt(42,2,n,10);
		}
		if(keydown(M_KEY)){
			printString(0,2,"         ");
			printString(0,3,"         ");
		}
		
		if(keydown(R_KEY)){
			n = readI2c();
			printString(0,3,"data");
			printInt(42,3,n,16);
		}	
	}

}

