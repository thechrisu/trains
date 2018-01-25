#include "test_task.h"
#include <gtest/gtest.h>

using namespace std;

#ifndef ALLTESTS
int oe_in_sensor = false;
#endif /* ALLTESTS */

TEST(TaskTest, init_sets_task_correctly) {
  task_descriptor first_task;
  send_queue send_queues_on_stack[MAX_TASKS];
  send_queues = (send_queue*)send_queues_on_stack;

  task_init(&first_task, 0, (void (*)())0xCAFEBABE, nullptr);
  ASSERT_EQ(first_task.tf->k_lr, (uint32_t)0xCAFEBABE);
  ASSERT_EQ(first_task.state, TASK_RUNNABLE);
  ASSERT_EQ(first_task.tf->psr, 0x10);

}

TEST(TaskTest, parents_set_correctly) {
  task_descriptor first_task, second_task;
  send_queue send_queues_on_stack[MAX_TASKS];
  send_queues = (send_queue*)send_queues_on_stack;

  task_init(&first_task, 0, (void (*)())0xCAFED00D, nullptr);
  task_init(&second_task, 0, (void (*)())nullptr, &(first_task));
  ASSERT_EQ(second_task.parent, &first_task);
}

TEST(TaskTest, stacks_dont_overlap_at_least_for_trapframe) {
  task_descriptor first_task, second_task;
  send_queue send_queues_on_stack[MAX_TASKS];
  send_queues = (send_queue*)send_queues_on_stack;

  task_init(&first_task, 0, (void (*)())nullptr, nullptr);
  task_init(&second_task, 0, (void (*)())nullptr, nullptr);
  uint64_t first_task_top = ((uint64_t)first_task.tf) + sizeof(trapframe);
  uint64_t second_task_top = ((uint64_t)second_task.tf) + sizeof(trapframe);
  ASSERT_NE(first_task.tf, second_task.tf);
  if (first_task_top > second_task_top) {
    ASSERT_GT((uint64_t)first_task.tf, second_task_top);
    ASSERT_GT(first_task.tf, second_task.tf); // obviously
  } else {
    ASSERT_LT((uint64_t)first_task.tf, second_task_top);
    ASSERT_LT(first_task.tf, second_task.tf); // obviously
  }
}

TEST(TaskTest, getters_task_id) {
  task_descriptor first_task, second_task;
  send_queue send_queues_on_stack[MAX_TASKS];
  send_queues = (send_queue*)send_queues_on_stack;

  task_init(&first_task, 0, (void (*)())nullptr, nullptr);
  task_init(&second_task, 0, (void (*)())nullptr, &first_task);
  ASSERT_EQ(task_get_parent_tid(&first_task), -1);
  ASSERT_EQ(task_get_parent_tid(&second_task), task_get_tid(&first_task));
}

TEST(TaskTest, task_ids_assigned_incrementally) {
  task_descriptor first_task, second_task;
  send_queue send_queues_on_stack[MAX_TASKS];
  send_queues = (send_queue*)send_queues_on_stack;

  task_init(&first_task, 0, (void (*)())nullptr, nullptr);
  task_init(&second_task, 0, (void (*)())nullptr, nullptr);
  ASSERT_EQ(task_get_tid(&first_task) + 1, task_get_tid(&second_task));
}

#ifndef ALLTESTS
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#endif /* ALLTESTS */
