//
// Created by Christoph Ulshöfer on 2018-01-05.
//

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

#if (HOSTCONFIG || TESTING) // x86-64 Googletest tests
#include <time.h>
void setup_timer();
uint32_t get_time();
uint32_t get_cached_time();
#else // Lab
#include "../labenv/timer.h"
#endif /* VERSATILEPB */
#endif /* TESTING */
#endif /* LAB */

#endif /* TRAINS_MYTIMER_H */
