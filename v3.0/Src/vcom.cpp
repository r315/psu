
#include <stdint.h>
#include <stdio.h>

#include "vcom.h"

extern "C" {
#include "usbd_cdc_if.h"
#include "strfunctions.h"
#include "fifo.h"

Fifo rxfifo;

}

void Vcom::init(void){
	fifoInit(&rxfifo);
}

char Vcom::getchar(void)
{
	int c = EOF;
	while(c == EOF)
	{
		fifoTake(&rxfifo, (uint8_t*)&c);
	}
	return (char)c;
}

uint8_t Vcom::checkForChar(char *c){
	
	return fifoTake(&rxfifo, (uint8_t*)c);
}

void putAndRetry(uint8_t *data, uint16_t len){
uint32_t retries = 1000;
	while(retries--){
		if(	CDC_Transmit_FS(data, len) == USBD_OK)
			break;
	}
}
void Vcom::putchar(char c){
	putAndRetry((uint8_t*)&c, 1);
}

void Vcom::puts(const char *s){
uint16_t len = 0;
	
	while( *((const char*)(s + len)) != '\0'){
		len++;	
	}
	putAndRetry((uint8_t*)s, len);
}
