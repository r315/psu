#ifndef _pcf8574_h_
#define _pcf8574_h_

#include <stdint.h>
#include "i2c.h"

#define PCF8574_I2C_ADDRESS   			0x20 // 7-bit address

void EXPANDER_Init(i2cbus_t *i2c);
uint8_t EXPANDER_Read(i2cbus_t *i2c);
void EXPANDER_Write(i2cbus_t *i2c, uint8_t data);

#endif