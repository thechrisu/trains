#include "test_messaging.h"

#include <gtest/gtest.h>

#ifndef ALLTESTS
int oe_in_sensor = false;
#endif /* ALLTESTS */

TEST(MessagePassingTest, send_blocks_a_task_if_receiver_isnt_zombie_or_receive_blocked) {
  setup_scheduler();
  task_descriptor sender, receiver;
  send_queue send_queues_on_stack[MAX_TASKS];
  send_queues = (send_queue*)send_queues_on_stack;
  task_init(&sender, 1, nullptr, nullptr);
  task_init(&receiver, 1, nullptr, nullptr);

  int sender_tid;
  receiver.tf->r1 = (register_t)&sender_tid;
  send(&sender, &receiver);
  ASSERT_EQ(sender.state, TASK_SEND_BLOCKED);
}

TEST(MessagePassingTest, on_receiver_task_blocked_actually_copies_buf) {
  task_descriptor all_tasks_on_stack[MAX_TASKS];
  all_tasks = (task_descriptor*)all_tasks_on_stack;
  send_queue send_queues_on_stack[MAX_TASKS];
  send_queues = (send_queue*)send_queues_on_stack;
  setup_scheduler();

  task_descriptor sender, receiver;
  task_init(&sender, 1, nullptr, nullptr);
  task_init(&receiver, 1, nullptr, nullptr);

  char send_buf[11] = "ABCDEFGHIJ";
  char receive_buf[11] = "XXXXXXXXXX";
  sender.tf->r2 = (register_t)send_buf;
  int elems_to_copy = 8;
  sender.tf->r3 = elems_to_copy;

  int sender_tid;
  receiver.tf->r1 = (register_t)&sender_tid;
  receiver.tf->r2 = (register_t)receive_buf;
  receiver.tf->r3 = elems_to_copy;
  task_set_state(&receiver, TASK_RECEIVE_BLOCKED);

  send(&sender, &receiver);
  ASSERT_EQ(receiver.tf->r0, elems_to_copy);
  ASSERT_EQ(sender_tid, task_get_tid(&sender));
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
  send_queue send_queues_on_stack[MAX_TASKS];
  send_queues = (send_queue*)send_queues_on_stack;
  setup_scheduler();

  task_descriptor sender, receiver;
  task_init(&sender, 1, nullptr, nullptr);
  task_init(&receiver, 1, nullptr, nullptr);

  char send_buf[11] = "ABCDEFGHIJ";
  char receive_buf[11] = "XXXXXXXXXX";
  sender.tf->r2 = (register_t)send_buf;
  sender.tf->r3 = 15;


  int sender_tid;
  receiver.tf->r1 = (register_t)&sender_tid;
  receiver.tf->r2 = (register_t)receive_buf;
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
  send_queue send_queues_on_stack[MAX_TASKS];
  send_queues = (send_queue*)send_queues_on_stack;
  setup_scheduler();

  task_descriptor sender, receiver;
  task_init(&sender, 1, nullptr, nullptr);
  task_init(&receiver, 1, nullptr, nullptr);

  char send_buf[10];
  sender.tf->r2 = (register_t)send_buf;
  sender.tf->r3 = 10;


  int sender_tid;
  receiver.tf->r1 = (register_t)&sender_tid;
  receiver.tf->r2 = (register_t)send_buf;
  receiver.tf->r3 = 10;
  task_set_state(&receiver, TASK_RECEIVE_BLOCKED);

  send(&sender, &receiver);
  ASSERT_EQ(sender.state, TASK_REPLY_BLOCKED);
  ASSERT_EQ(receiver.state, TASK_RUNNABLE);
}

TEST(MessagePassingTest, test_send_receive_reply_happypath_sender_first) {
  task_descriptor all_tasks_on_stack[MAX_TASKS];
  all_tasks = (task_descriptor*)all_tasks_on_stack;
  send_queue send_queues_on_stack[MAX_TASKS];
  send_queues = (send_queue*)send_queues_on_stack;
  setup_scheduler();

  task_descriptor sender, receiver;
  task_init(&sender, 1, nullptr, nullptr);
  task_init(&receiver, 1, nullptr, nullptr);

  char send_send_buf[11] = "0123456789";
  char send_recv_buf[11];
  char recv_recv_buf[11];
  char recv_send_buf[11] = "REPLYREPL";
  sender.tf->r1 = task_get_tid(&receiver);
  sender.tf->r2 = (register_t)send_send_buf;
  sender.tf->r3 = 10;
  sender.tf->r4 = (register_t)send_recv_buf;
  sender.tf->r5 = 10;

  send(&sender, &receiver);

  int sender_tid;
  receiver.tf->r1 = (register_t)&sender_tid;
  receiver.tf->r2 = (register_t)recv_recv_buf;
  receiver.tf->r3 = 10;

  receive(&receiver);

  ASSERT_EQ(sender_tid, task_get_tid(&sender));
  ASSERT_EQ(receiver.tf->r0, 10);
  for (int i = 0; i < 10; i++) {
    ASSERT_EQ(recv_recv_buf[i], send_send_buf[i]);
  }

  receiver.tf->r2 = (register_t)recv_send_buf;
  receiver.tf->r3 = 10;

  reply(&sender, &receiver);

  ASSERT_EQ(sender.tf->r0, 10);
  ASSERT_EQ(receiver.tf->r0, 0);
  for (int i = 0; i < 10; i++) {
    ASSERT_EQ(((char *)sender.tf->r4)[i], recv_send_buf[i]);
  }
  ASSERT_EQ(sender.state, TASK_RUNNABLE);
  ASSERT_EQ(receiver.state, TASK_RUNNABLE);
}

#ifndef ALLTESTS
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#endif /* ALLTESTS */
