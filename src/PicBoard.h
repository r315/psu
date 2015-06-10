#ifndef _picboard_h_
#define _picboard_h_

#include <htc.h>
__CONFIG(0x3FBA);

#ifndef PIC_CLK
  #define PIC_CLK 8000000
#endif

#ifndef BPORT  
  #define BPORT     PORTA  
  #define BPORTMask 0x0f  //4 buttons
#endif

#define Button1 (1<<0)
#define Button2 (1<<1)
#define Button3 (1<<2)
#define Button4 (1<<3)

#define ALLDIGITAL 1
#define ANALOG 0

#define TMR1_EXT_CLK 0x02
#define TMR1_SYNC_OFF 0x04
#define TMR1_PS1 0x00
#define TMR1_PS2 0x10
#define TMR1_PS4 0x20
#define TMR1_PS8 0x30

typedef unsigned char byte;
typedef unsigned int uint;

void systemInit(char Digital);
char keydown(char k);
#endif

