#ifndef TEST_DESTROY_H
#define TEST_DESTROY_H

#include "codes.h"
#include "constants.h"
#include "nameserver.h"
#include "../nameserver/ns_tid.h"

void test_destroy_many_tasks();
void test_destroy_task_limit();
void test_destroy_nameserver();
void test_destroy_send_queue();
void test_destroy_block_kill();
void test_destroy_parent_tid();

#endif /* TEST_DESTROY_H */
