
#include "fifo.h"

void fifoInit(Fifo *fifo){
    fifo->head = fifo->tail = 0;
}

void fifoPut(Fifo *fifo, uint8_t itm){
	uint32_t next;
	
	next = (fifo->head + 1);
	if(next >= fifo->size) next -= fifo->size;

	if (next == fifo->tail) return;
	
	fifo->buf[fifo->head] = itm;
	fifo->head = next;
}

void fifoPutBuf(Fifo *fifo, uint8_t *buf, uint32_t len){
	while(len--)
		fifoPut(fifo, *buf++);
}

uint8_t fifoTake(Fifo *fifo, uint8_t *itm)
{
	uint32_t next;
	
	if (fifo->head == fifo->tail) return 0;
	
	next = (fifo->tail + 1);
	if(next >= fifo->size) next -= fifo->size;
	
	*itm = fifo->buf[fifo->tail];
	fifo->tail = next;

	return 1;
}

uint32_t fifoAvailable(Fifo *fifo)
{
    uint32_t size = fifo->size + fifo->head - fifo->tail;
    if (size >= fifo->size) size -= fifo->size;
    return size;
}