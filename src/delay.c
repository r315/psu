
#include <htc.h>

#define DelayDivisor 2
#define WaitFor1Us asm("nop")
#define Jumpback asm("goto $ - 2")

#define DelayUs(x) { \
			delayus_variable=(unsigned char)(x/DelayDivisor); \
			WaitFor1Us; } \
			asm("decfsz _delayus_variable,f"); \
			Jumpback;
			
			
void delayMs(unsigned char cnt)
{
unsigned char	i;
    do {
	i = 4;
	do {          
            asm("nop");
            asm("nop");
            CLRWDT();
	} while(--i);
    } while(--cnt);
}


