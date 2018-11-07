
#include <stdint.h>
#include <stdio.h>

#include "vcom.h"
extern "C" {
#include "usbd_cdc_if.h"
#include "strfunctions.h"
}

void Vcom::init(void)
{
}

char Vcom::getc()
{
    uint32_t len;
	int c;
	c = EOF;
	while(c == EOF)
	{
		if(CDC_Receive_FS((uint8_t*)&c, &len) != USBD_OK)
            return EOF;
	}
	return (char)c;
}

void Vcom::putc(char c){
	CDC_Transmit_FS((uint8_t*)&c, 1);
}

void Vcom::putString(const char *s, uint8_t len){
	CDC_Transmit_FS((uint8_t*)s, len);
}
