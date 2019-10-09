
#include <stdint.h>
#include "board.h"

void EXPANDER_Init(void){

}

uint8_t EXPANDER_Read(void){
uint8_t data;
    I2C_Read(PCF8574_I2C_ADDRESS, &data, 1);
    return data;
}

void EXPANDER_Write(uint8_t data){
    I2C_Write(PCF8574_I2C_ADDRESS, &data, 1);
}