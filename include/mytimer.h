/**
 * @file
 * @brief (Wrapper) Functions for timer devices
 */
#ifndef TRAINS_MYTIMER_H
#define TRAINS_MYTIMER_H

#include "timer_data.h"

#if VERSATILEPB // QEMU
#include "../kernel/include/versatilepb/timer.h"
#include "stdlib.h"

#else


#if (HOSTCONFIG || TESTING) // x86-64 Googletest tests
#include <time.h>
#include <stdint.h>
void setup_timer();
uint32_t get_time();
uint32_t get_cached_time();
#else // Lab
#include "../kernel/include/labenv/timer.h"
#endif /* LAB */
#endif /* VERSATILEPB */

#endif /* TRAINS_MYTIMER_H */
