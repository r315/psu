#ifndef _eeprom_h_
#define _eeprom_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define EEPROM_I2C_ADDRESS      0x50    //7-bit address
#define EEPROM_SIZE             256
#define EEPROM_PAGE_SIZE        8
#define EEPROM_TWR              5       // ms

void EEPROM_Init(void);

uint16_t EEPROM_Read(uint16_t address, uint8_t *dst, uint16_t count);
uint16_t EEPROM_Write(uint16_t address, uint8_t *src, uint16_t count);
uint16_t EEPROM_Erase(void);

#ifdef __cplusplus
}
#endif

#endif /* _eeprom_h_ */