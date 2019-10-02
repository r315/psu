/********************************************
 general spi functions for pic16fxxx family
********************************************/
#include <htc.h>
#include "spi.h"

//----------------------------------------
//
//----------------------------------------
void spiInit(char m)
{
    switch(m){
		default:
		case SPI_MODE0:
			SSPSTAT= CKE_P;
			SSPCON = SPI_CLK16 | SPI_ON;
			break;
		
		case SPI_MODE1:
			SSPSTAT= CKE_P;
			SSPCON = SPI_CLK16 | CKP_P | SPI_ON;
			break;
		
		case SPI_MODE2:
			SSPSTAT= 0;
			SSPCON = SPI_CLK16 | SPI_ON;	
			break;
			
		case SPI_MODE3:
			SSPSTAT= 0;
			SSPCON = SPI_CLK16 | CKP_P | SPI_ON;
			break;		
	}
	SPI(0xFF);
}
//----------------------------------------
//
//----------------------------------------
char SPI(char data)
{
	SSPBUF = data;
	while(!BF);
	return SSPBUF;
}
