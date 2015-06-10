#ifndef _SPI_H_
#define _SPI_H_

#define SPI_CLK4  0     // Fosc/4
#define SPI_CLK16 (1<<0)
#define SPI_CLK64 (1<<1)

#define IDLE_H (1<<4) // clock idles at high (CKP)
#define SSP_E  (1<<5) // Enable ssp module 
#define EDGE_R (1<<6) // Transmit on rising edge (CKE)

void spiInit(void);
char SPI(char data);
#endif
