#ifndef _fifo_h_
#define _fifo_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define FIFO_SIZE   256

typedef struct {
    volatile uint8_t buf[FIFO_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
    uint16_t size;
}Fifo;

void fifoPut(Fifo *fifo, uint8_t itm);
void fifoPutBuf(Fifo *fifo, uint8_t *buf, uint32_t len);
uint8_t fifoTake(Fifo *fifo, uint8_t *itm);
uint32_t fifoAvailable(Fifo *fifo);
void fifoInit(Fifo *fifo);

#ifdef __cplusplus
}
#endif

#endif