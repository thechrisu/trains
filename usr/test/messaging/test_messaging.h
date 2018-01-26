#ifndef TEST_MESSAGING_H
#define TEST_MESSAGING_H

#include "myio.h"
#include "../../../lib/project/codes.h"

void test_messaging_basic();
void test_messaging_receive_before_send();
void test_messaging_sequence();
void test_messaging_truncation();
void test_messaging_invalid_tid();
void test_messaging_fifo_send();
void test_messaging_same_priority();
void test_messaging_send_recipient_zombie();
void test_messaging_reply_target_zombie();
void test_messaging_tree();
void test_messaging_exit_with_blocked();

#endif /* TEST_MESSAGING_H */
