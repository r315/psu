#ifndef _RTC_H_
#define _RTC_H_


#define CONST_1S 123

void rtcInit(void);
void startCounting(void);
void stopCounting(void);
void getTime(char *dst);
int getSeconds(void);
char hasUpdate(void);

#endif
