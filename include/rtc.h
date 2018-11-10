#ifndef _RTC_H_
#define _RTC_H_

#define false 0
#define true 1
#define CONST_1S 123

void rtcInit(void);
void startCounting(void);
void stopCounting(void);
void getTime(char *dst);
int getSeconds(void);
char timeChange(void);

#endif
