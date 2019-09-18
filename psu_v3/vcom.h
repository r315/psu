#ifndef _vcom_h_
#define _vcom_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <fifo.h>

#define VCOM_FIFO_SIZE 512

extern fifo_t rxfifo;

#ifdef __cplusplus
}
#endif

#endif
