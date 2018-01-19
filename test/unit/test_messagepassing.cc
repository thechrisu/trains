#include "test_messagepassing.h"

#include <gtest/gtest.h>

extern task_descriptor *current_task;

int oe_in_sensor = false;

TEST(MessagePassingTest, send_blocks_a_task_if_receiver_isnt_zombie_or_receive_blocked) {
  setup_scheduler();
  task_descriptor sender, receiver;
  task_init(&sender, 1, nullptr, nullptr);
  task_init(&receiver, 1, nullptr, nullptr);
  send(&sender, &receiver);
  ASSERT_EQ(sender.state, TASK_SEND_BLOCKED);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
// TEST(MessagePassingTest, send)