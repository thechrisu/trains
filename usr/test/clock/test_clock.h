/**
 * @file
 * @brief E2E tests for the clock server and notifier
 */
#ifndef TEST_CLOCK_H
#define TEST_CLOCK_H

#include "clock.h"
#include "codes.h"
#include "idle.h"
#include "messages.h"
#include "nameserver.h"
#include "myio.h"
#include "mytimer.h"
#include "tstdlib.h"
#include "../nameserver/fake_nameserver_functions.h"

/**
 * Assert that the given number of milliseconds is within a range of ten-millisecond ticks
 * around the correct tick count.
 *
 * @param milliseconds The number of milliseconds to test.
 * @param ticks        The number of ticks to test against.
 * @param below        The number of ticks by which the number of milliseconds can be less
 *                     than expected.
 * @param above        The number of ticks by which the number of milliseconds can be greater
 *                     than expected.
 */
void assert_within_tick_range(uint32_t milliseconds, uint32_t ticks, int below, int above);

void clock_errors();
void clock_accuracy();
void clock_syscall_errors();
void clock_syscall_accuracy();

#endif /* TEST_CLOCK_H */
