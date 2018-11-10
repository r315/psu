#ifndef _SPI_H_
#define _SPI_H_

#define SPI_CLK4  0
#define SPI_CLK16 1
#define SPI_CLK64 2
#define CKP_P	0x10 // clock idles at high (CKP)
#define CKE_P	0x40 // Transmit on rising edge (CKE)
#define SPI_ON	0x20 // Enable ssp module 
#define SPI_SMP (1<<7) //Input sampled at end of data

#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3
/*
+--------+-----------+-------------------------------------------+
|SPI BUS | CONTROL   |                                           |
|MODE    | CKP | CKE | Description                               |
|--------+-----+-----+-------------------------------------------+
|   0    |  0  |  1  | Clock idle low, transmit on rising edge   |
|   1    |  1  |  1  | Clock idle high, transmit on falling edge |
|   2    |  0  |  0  | Clock idle low, transmit on falling edge  |
|   3    |  1  |  0  | Clock idle high, transmit on rising edge  |
+--------+-----+-----+-------------------------------------------+  
*/

void spiInit(char m);
char SPI(char data);
#endif
