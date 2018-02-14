/**
 * @file
 * @brief Tests that AwaitEvent on the receive interrupt from the terminal works.
 */
#ifndef TEST_RX_TERMINAL
#define TEST_RX_TERMINAL

#include "assert.h"
#include "codes.h"
#include "event_data.h"
#include "myio.h"
#include "rawio.h"
#include "../../idle.h"
#include "../../nameserver.h"
#include "../nameserver/ns_tid.h"

void test_rx_terminal();

#endif /* TEST_RX_TERMINAL */
