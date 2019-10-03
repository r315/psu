#ifndef _psu_h_
#define _psu_h_


#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "layout_ui.h"
#include "text.h"
#include <console.h>
#include "cmdhelp.h"
#include "cmdadc.h"
#include "cmdpwm.h"
#include "cmddfu.h"
#include "cmdpwr.h"

#define ADC_INTERVAL 100
#define APP_INTERVAL 10

static const uint16_t pwm_start_values [] = { 0x80, 0x180, 0x280, 0x380};

#ifdef __cplusplus
}
#endif

#endif