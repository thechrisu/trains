#ifndef TEST_MESSAGING_H
#define TEST_MESSAGING_H

#include "../../include/common/codes.h"
#include "myio.h"

void test_messaging_basic();
void test_messaging_receive_before_send();
void test_messaging_sequence();
void test_messaging_truncation();
void test_messaging_invalid_tid();

#endif /* TEST_MESSAGING_H */
