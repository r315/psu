
#include <stdint.h>
#include <stdout.h>
#include <fifo.h>
#include "vcom.h"
#include "usbd_cdc_if.h"

fifo_t rxfifo;

void vc_init(void){
	rxfifo.size = VCOM_FIFO_SIZE;
	fifo_init(&rxfifo);
	fifo_flush(&rxfifo);
}

uint8_t vc_getCharNonBlocking(char *c){
	return fifo_get(&rxfifo, (uint8_t*)c);
}

char vc_getchar(void){
	char c;
	while(!fifo_get(&rxfifo, (uint8_t*)&c));	
	return c;
}

uint8_t vc_kbhit(void){
    return fifo_avail(&rxfifo);
}

void putAndRetry(uint8_t *data, uint16_t len){
uint32_t retries = 1000;
	while(retries--){
		if(	CDC_Transmit_FS(data, len) == USBD_OK)
			break;
	}
}

void vc_putchar(char c){
	putAndRetry((uint8_t*)&c, 1);
}

void vc_puts(const char *s){
uint16_t len = 0;
	
	while( *((const char*)(s + len)) != '\0'){
		len++;	
	}
	putAndRetry((uint8_t*)s, len);
}

StdOut vcom = {
    .init = vc_init,
    .xgetchar = vc_getchar,
    .xputchar = vc_putchar,
    .xputs = vc_puts,
    .getCharNonBlocking = vc_getCharNonBlocking,
    .kbhit = vc_kbhit
};


