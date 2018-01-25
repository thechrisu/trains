#include "test_scheduler.h"
#include <gtest/gtest.h>

using namespace std;

TEST(SchedulerTest, init_returns_negative_one_if_max_priority_is_negative) {
  scheduler s;
  ASSERT_EQ(scheduler_init(&s, -1, (ready_queue *)0), -1);
}

TEST(SchedulerTest, init_initializes_a_scheduler_with_one_queue) {
  scheduler s;
  ready_queue rq[1];

  ASSERT_EQ(scheduler_init(&s, 0, rq), 0);
  ASSERT_EQ(s.max_priority, 0);
  ASSERT_EQ(s.queues[0], NULL_READY_QUEUE);
}

TEST(SchedulerTest, init_initializes_a_scheduler_with_five_queues) {
  scheduler s;
  ready_queue rq[5];
  int i;

  ASSERT_EQ(scheduler_init(&s, 4, rq), 0);
  ASSERT_EQ(s.max_priority, 4);
  for (i = 0; i < 5; i += 1)
    ASSERT_EQ(s.queues[i], NULL_READY_QUEUE);
}

TEST(SchedulerTest, register_returns_negative_one_if_task_priority_is_negative) {
  scheduler s;
  ready_queue rq[1];
  task_descriptor td;
  td.priority = -1;

  scheduler_init(&s, 0, rq);
  ASSERT_EQ(scheduler_register(&s, &td), -1);
}

TEST(SchedulerTest, register_returns_negative_one_if_task_priority_is_greater_than_max_priority) {
  scheduler s;
  ready_queue rq[1];
  task_descriptor td;
  td.priority = 1;

  scheduler_init(&s, 0, rq);
  ASSERT_EQ(scheduler_register(&s, &td), -1);
}

TEST(SchedulerTest, register_adds_task_to_the_correct_queue) {
  scheduler s;
  ready_queue rq[3];
  task_descriptor td;
  td.priority = 1;

  scheduler_init(&s, 2, rq);
  ASSERT_EQ(scheduler_register(&s, &td), 0);
  ASSERT_EQ(s.queues[0], NULL_READY_QUEUE);
  ASSERT_EQ(s.queues[1], &td);
  ASSERT_EQ(s.queues[2], NULL_READY_QUEUE);
}

TEST(SchedulerTest, next_task_returns_null_task_descriptor_if_all_queues_are_empty) {
  scheduler s;
  ready_queue rq[3];
  task_descriptor td1, td2;
  td1.priority = 1;
  td2.priority = 2;

  scheduler_init(&s, 2, rq);
  ASSERT_EQ(scheduler_next_task(&s), NULL_TASK_DESCRIPTOR);

  scheduler_register(&s, &td1);
  scheduler_register(&s, &td2);
  scheduler_next_task(&s);
  scheduler_next_task(&s);
  ASSERT_EQ(scheduler_next_task(&s), NULL_TASK_DESCRIPTOR);
}

TEST(SchedulerTest, next_task_returns_task_from_highest_priority_non_empty_queue) {
  scheduler s;
  ready_queue rq[10];
  task_descriptor td[5];
  int i;

  scheduler_init(&s, 9, rq);
  for (i = 0; i < 5; i += 1) {
    td[i].priority = 2 * i + 1;
    scheduler_register(&s, &(td[i]));
  }

  for (i = 4; i >= 0; i -= 1)
    ASSERT_EQ(scheduler_next_task(&s), &(td[i]));
}

TEST(SchedulerTest, next_task_has_fifo_ordering) {
  scheduler s;
  ready_queue rq[1];
  task_descriptor td[10];
  int i;

  scheduler_init(&s, 0, rq);
  for (i = 0; i < 10; i += 1) {
    td[i].priority = 0;
    scheduler_register(&s, &(td[i]));
  }

  for (i = 0; i < 10; i += 1)
    ASSERT_EQ(scheduler_next_task(&s), &(td[i]));
}

#ifndef ALLTESTS
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#endif /* ALLTESTS */
