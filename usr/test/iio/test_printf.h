/**
 * @file
 * @brief Tests for Printf.
 */
#ifndef TEST_PRINTF_H
#define TEST_PRINTF_H

#include "codes.h"
#include "clock.h"
#include "idle.h"
#include "ioserver.h"
#include "nameserver.h"
#include "../nameserver/ns_tid.h"

void printf_errors();
void printf_happy_path();

#endif /* TEST_PRINTF_H */
