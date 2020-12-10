#include <string.h>
#include "eeprom.h"
#include "board.h"

#ifdef ENABLE_EEPROM

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
    uint8_t buf[EEPROM_PAGE_SIZE + 1], chunk, remaning;
    
    if(address > 255){
        return 0;
    }

    buf[0] = address & 255;
    remaning = count;

    while(remaning){
        if(remaning > EEPROM_PAGE_SIZE){
            chunk = EEPROM_PAGE_SIZE;
        }else{            
            chunk = remaning;
        }

        memcpy(buf + 1, src, chunk);
        if(!I2C_Write(EEPROM_I2C_ADDRESS, buf, chunk + 1)){
            return count - remaning;
        }
        DelayMs(EEPROM_TWR);
        remaning -= chunk;
        src += chunk;
        buf[0] += chunk;
    }

    return count;
}

uint16_t EEPROM_Erase(void){
    uint8_t dummy[EEPROM_PAGE_SIZE];

    memset(dummy, 0xFF, EEPROM_PAGE_SIZE);

    for(uint16_t i = 0; i < EEPROM_SIZE; i += EEPROM_PAGE_SIZE){
        if(EEPROM_Write(i, dummy, EEPROM_PAGE_SIZE) != EEPROM_PAGE_SIZE){
            return 0;
        }
    }
    return EEPROM_SIZE;
}

#endif
