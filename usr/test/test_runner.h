/**
 * @file
 * @brief Function us to dynamically call different main functions.
 */
#ifndef TEST_RUNNER
#define TEST_RUNNER

#include "../a0.h"
#include "../k1.h"
#include "../k2.h"
#include "../k3.h"
#include "./messaging/test_messaging.h"
#include "nameserver/test_nameserver_errors.h"
#include "nameserver/test_nameserver_wrapper_errors.h"
#include "nameserver/test_nameserver_too_many.h"
#include "nameserver/test_nameserver_happypath.h"
#include "awaitevent/test_timer_interrupt.h"
#include "./iio/test_getcputc_errors.h"
#include "./iio/test_getcputc_happypath.h"
#include "./test_undefined_handler.h"
#include "./test_message_benchmark.h"
#include "./test_mypriority.h"
#include "clock/test_clock.h"
#include "myio.h"
#include "stdlib.h"

void test_runner();

#endif /* TEST_RUNNER */
