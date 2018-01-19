#include "test_messagepassing.h"

#include <gtest/gtest.h>

#ifndef ALLTESTS
int oe_in_sensor = false;
#endif /* ALLTESTS */

TEST(MessagePassingTest, send_blocks_a_task_if_receiver_isnt_zombie_or_receive_blocked) {
  setup_scheduler();
  task_descriptor sender, receiver;
  task_init(&sender, 1, nullptr, nullptr);
  task_init(&receiver, 1, nullptr, nullptr);
  send(&sender, &receiver);
  ASSERT_EQ(sender.state, TASK_SEND_BLOCKED);
}

TEST(MessagePassingTest, on_receiver_task_blocked_actually_copies_buf) {
  task_descriptor all_tasks_on_stack[MAX_TASKS];
  all_tasks = (task_descriptor*)all_tasks_on_stack;
  setup_scheduler();
  task_descriptor sender, receiver;
  task_init(&sender, 1, nullptr, nullptr);
  task_init(&receiver, 1, nullptr, nullptr);
  char send_buf[11] = "ABCDEFGHIJ";
  char receive_buf[11] = "XXXXXXXXXX";
  sender.tf->r2 = (uint64_t)send_buf;
  int elems_to_copy = 8;
  sender.tf->r3 = elems_to_copy;
  receiver.tf->r2 = (uint64_t)receive_buf;
  receiver.tf->r3 = elems_to_copy;
  task_set_state(&receiver, TASK_RECEIVE_BLOCKED);
  send(&sender, &receiver);
  ASSERT_EQ(receiver.tf->r0, elems_to_copy);
  char expected_buf[11] = "ABCDEFGHXX";
  for (int i = 0; i < 10; i++) {
  ASSERT_EQ(receive_buf[i], expected_buf[i]);
  }
}

// TODO test send->receive mechanics
/*
 * FIFO order
 * What if we receive, but there's no send?
 * Receive -> send -> Receive
 */

TEST(MessagePassingTest, on_receiver_task_blocked_if_sender_message_too_long_receive_will_see_that_message_is_truncated) {
  task_descriptor all_tasks_on_stack[MAX_TASKS];
  all_tasks = (task_descriptor*)all_tasks_on_stack;
  setup_scheduler();
  task_descriptor sender, receiver;
  task_init(&sender, 1, nullptr, nullptr);
  task_init(&receiver, 1, nullptr, nullptr);
  char send_buf[11] = "ABCDEFGHIJ";
  char receive_buf[11] = "XXXXXXXXXX";
  sender.tf->r2 = (uint64_t)send_buf;
  sender.tf->r3 = 15;
  receiver.tf->r2 = (uint64_t)receive_buf;
  receiver.tf->r3 = 8;
  task_set_state(&receiver, TASK_RECEIVE_BLOCKED);
  send(&sender, &receiver);
  ASSERT_EQ(receiver.tf->r0, -1);
  char expected_buf[11] = "ABCDEFGHXX";
  for (int i = 0; i < 10; i++) {
    ASSERT_EQ(receive_buf[i], expected_buf[i]);
  }
}

TEST(MessagePassingTest, on_receiver_task_blocked_states_set_correctly) {
  task_descriptor all_tasks_on_stack[MAX_TASKS];
  all_tasks = (task_descriptor*)all_tasks_on_stack;
  setup_scheduler();
  task_descriptor sender, receiver;
  task_init(&sender, 1, nullptr, nullptr);
  task_init(&receiver, 1, nullptr, nullptr);
  char send_buf[10];
  sender.tf->r2 = (uint64_t)send_buf;
  sender.tf->r3 = 10;
  receiver.tf->r2 = (uint64_t)send_buf;
  receiver.tf->r3 = 10;
  task_set_state(&receiver, TASK_RECEIVE_BLOCKED);
  send(&sender, &receiver);
  ASSERT_EQ(sender.state, TASK_REPLY_BLOCKED);
  ASSERT_EQ(receiver.state, TASK_RUNNABLE);
}

#ifndef ALLTESTS
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#endif /* ALLTESTS */
