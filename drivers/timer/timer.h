#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>
#include "io.h"

void init_timer(void);

void isr_clock_int(void);

void wait(uint32_t t);

uint8_t second(void);

extern volatile uint32_t __counter;

#endif