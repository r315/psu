#ifndef _eeprom_h_
#define _eeprom_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "i2c.h"

#define EEPROM_I2C_ADDRESS      0x50    //7-bit address
#define EEPROM_SIZE             256
#define EEPROM_PAGE_SIZE        8
#define EEPROM_TWR              5       // ms

#define EEPROM_APP_OFFSET       0x00

void EEPROM_Init(i2cbus_t *i2c);

uint16_t EEPROM_Read(i2cbus_t *i2c, uint16_t address, uint8_t *dst, uint16_t count);
uint16_t EEPROM_Write(i2cbus_t *i2c, uint16_t address, const uint8_t *src, uint16_t count);
uint16_t EEPROM_Erase(i2cbus_t *i2c);

#ifdef __cplusplus
}
#endif

#endif /* _eeprom_h_ */