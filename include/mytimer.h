/**
 * @file
 * @brief (Wrapper) Functions for timer devices
 */
#ifndef TRAINS_MYTIMER_H
#define TRAINS_MYTIMER_H

#include "timer_data.h"

#if VERSATILEPB // QEMU
#include "../../../src/stdlib.h"
#include "../versatilepb/timer.h"

#else
#if TESTING // x86-64 Googletest tests
#else

#include "../../../src/stdlib.h"

#endif /* VERSATILEPB */

#if (HOSTCONFIG || TESTING) // x86-64 Googletest tests
#include <time.h>
void setup_timer();
uint32_t get_time();
uint32_t get_cached_time();
#else // Lab
#include "../labenv/timer.h"
#endif /* LAB */
#endif /* TESTING */

#endif /* TRAINS_MYTIMER_H */
