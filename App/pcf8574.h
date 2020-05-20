#ifndef _pcf8574_h_
#define _pcf8574_h_

#include <stdint.h>

#define PCF8574_I2C_ADDRESS   			0x20 // 7-bit address

void EXPANDER_Init(void);
uint8_t EXPANDER_Read(void);
void EXPANDER_Write(uint8_t data);

#endif