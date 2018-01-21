/**
 * @file
 * @brief Function us to dynamically call different main functions.
 */
#ifndef TEST_RUNNER
#define TEST_RUNNER

#include "../../include/common/codes.h"
#include "../a0.h"
#include "../k1.h"
#include "./messaging/test_messaging_basic.h"
#include "./messaging/test_messaging_receive_before_send.h"
#include "./messaging/test_messaging_sequence.h"
#include "./messaging/test_messaging_truncation.h"
#include "myio.h"
#include "stdlib.h"

void test_runner();

#endif /* TEST_RUNNER */
