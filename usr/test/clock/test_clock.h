/**
 * @file
 * @brief E2E tests for the clock server and notifier
 */
#ifndef TEST_CLOCK_H
#define TEST_CLOCK_H

#include "clock.h"
#include "idle.h"
#include "messages.h"
#include "nameserver.h"
#include "myio.h"
#include "mytimer.h"
#include "tstdlib.h"
#include "../nameserver/fake_nameserver_functions.h"

void clock_errors();
void clock_accuracy();

#endif /* TEST_CLOCK_H */
