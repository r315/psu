#ifndef _cmd_util_h_
#define _cmd_util_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "console.h"


char *getOptValue(const char *opt, uint32_t argc, char **argv);
int readIntParameter(const char *opt, uint32_t argc, char **argv, uint16_t *dst, void (*func)(uint32_t));
int readFloatParameter(const char *opt, uint32_t argc, char **argv, uint16_t *dst, void (*func)(float));


#ifdef __cplusplus
}
#endif

#endif