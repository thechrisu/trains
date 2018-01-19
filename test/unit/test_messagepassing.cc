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


TEST(MessagePassingTest, on_receiver_task_blocked_if_sender_message_too_long_receive_will_see_that_message_is_truncatedsend_blocks_a_task_if_receiver_isnt_zombie_or_receive_blocked) {
  setup_scheduler();
  task_descriptor sender, receiver;
  task_init(&sender, 1, nullptr, nullptr);
  task_init(&receiver, 1, nullptr, nullptr);
  send(&sender, &receiver);
  ASSERT_EQ(sender.state, TASK_SEND_BLOCKED);
}

#ifndef ALLTESTS
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#endif /* ALLTESTS */
