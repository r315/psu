#ifndef _seven_seg_h_
#define _seven_seg_h_

#include <stdint.h>

void SEVEN_Double(uint16_t x, uint16_t y, double val);
void SEVEN_Init(void);
void SEVEN_PrintAmps(double amps);
void SEVEN_PrintVolts(double volts);

#endif