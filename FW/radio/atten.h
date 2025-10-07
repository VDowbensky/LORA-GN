#ifndef _ATTEN_H_
#define _ATTEN_H_

#include "bsp.h"

void atten_init(void);
void atten_set(uint8_t atten,uint8_t value);
void set_attenuation(float val);

extern uint8_t atten[];

#endif
