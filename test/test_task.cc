#include "test_task.h"
#include <gtest/gtest.h>

TEST(TaskTest, init_sets_task_correctly) {
  task_descriptor first_task, second_task;
  task_init(&first_task, 0, (void (*)())0xCAFED00D, nullptr);
  task_init(&second_task, 0, (void (*)())nullptr, &first_task);
  ASSERT_EQ(all_tasks[first_task.tid], &first_task);
  ASSERT_EQ(all_tasks[second_task.tid], &second_task);
  ASSERT_EQ(second_task.parent, &first_task);
  ASSERT_EQ(first_task.tf->lr, (uint32_t)0xCAFED00D);
}

TEST(TaskTest, stacks_dont_overlap_at_least_for_trapframe) {
  task_descriptor first_task, second_task;
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
  task_init(&first_task, 0, (void (*)())nullptr, nullptr);
  task_init(&second_task, 0, (void (*)())nullptr, &first_task);
  ASSERT_EQ(task_get_parent_tid(&first_task), -1);
  ASSERT_EQ(task_get_parent_tid(&second_task), task_get_tid(&first_task));
}

TEST(TaskTest, task_ids_assigned_incrementally) {
  task_descriptor first_task, second_task;
  task_init(&first_task, 0, (void (*)())nullptr, nullptr);
  task_init(&second_task, 0, (void (*)())nullptr, nullptr);
  ASSERT_EQ(task_get_tid(&first_task) + 1, task_get_tid(&second_task));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}