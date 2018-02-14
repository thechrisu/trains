/**
 * @file
 * @brief Tests that AwaitEvent triggers when we get a timer interrupt.
 */
#ifndef TEST_TIMER_INTERRUPT_H
#define TEST_TIMER_INTERRUPT_H

#include "assert.h"
#include "codes.h"
#include "event_data.h"
#include "myio.h"
#include "../../idle.h"
#include "../../nameserver.h"
#include "../nameserver/ns_tid.h"

void test_timer_interrupt();

#endif /* TEST_TIMER_INTERRUPT_H */
