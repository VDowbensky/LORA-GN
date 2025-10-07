#ifndef _TXLEVEL_H_
#define _TXLEVEL_H_

#include "bsp.h"
#include "radio_proc.h"
#include "atten.h"

#define FIXED_ATTENUATION			60.0f
#define SOURCE_LEVEL					-10.0f
#define RF_LEVEL_DEFAULT			(SOURCE_LEVEL - FIXED_ATTENUATION)

void setrflevel(void);

extern float rflevel;
extern int8_t txpower;
extern uint8_t dutycycle;
extern uint8_t hpmax;

#endif

