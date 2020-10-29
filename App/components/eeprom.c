#include "eeprom.h"
#include "board.h"


void EEPROM_Init(void){
    uint8_t data;
    I2C_Read(EEPROM_I2C_ADDRESS, &data, 1);
}

uint16_t EEPROM_Read(uint16_t address, uint8_t *dst, uint16_t count){
    if(address > 255 || !I2C_Write(EEPROM_I2C_ADDRESS, (uint8_t*)&address, 1)){
        return 0;
    }   
    
    return I2C_Read(EEPROM_I2C_ADDRESS, dst, count);
}

uint16_t EEPROM_Write(uint16_t address, uint8_t *src, uint16_t count){
    if(address > 255 || !I2C_Write(EEPROM_I2C_ADDRESS, (uint8_t*)&address, 1)){
        return 0;
    }

    I2C_Write(EEPROM_I2C_ADDRESS, (uint8_t*)&address, 1);
    return I2C_Write(EEPROM_I2C_ADDRESS, src, count);
}