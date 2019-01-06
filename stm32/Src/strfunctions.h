#ifndef _strfunctions_h_
#define _strfunctions_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

char *nextParameter(char *line);
int32_t nextInt(char **line);
uint8_t nextHex(char **line, uint32_t *value);
char nextChar(char **line);
char *strtok_s(char *str, const char token, uint8_t len, char **saveptr);
char xstrcmp(char const *str1, char const *str2);
int yatoi(char *str);
uint8_t hatoi(char *str, uint32_t *value);
int xatoi (char **str, long *res);
char *pftoa(double f, char places);
char* pitoa (long val, int radix, int len);
char * strchr ( const char *str, int c);
size_t strlen(const char *str);

#ifdef __cplusplus
}
#endif
#endif

