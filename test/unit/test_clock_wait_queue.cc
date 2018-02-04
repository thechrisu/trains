#include "test_clock_wait_queue.h"
#include <gtest/gtest.h>

using namespace std;

void populate_queue(clock_wait_queue *q) {
  clock_wait cw;
  uint32_t ticks[CLOCK_WAIT_QUEUE_SIZE] = { 555, 231, 789, 111, 23, 1200, 232, 10 };

  for (int i = 0; i < CLOCK_WAIT_QUEUE_SIZE; i += 1) {
    cw.tid = i + 1;
    cw.ticks = ticks[i];
    clock_wait_queue_enqueue(q, &cw);
  }
}

TEST(ClockWaitQueueTest, init_sets_size_to_zero) {
  clock_wait_queue q;
  clock_wait_queue_init(&q);
  ASSERT_EQ(0, q.size);
}

TEST(ClockWaitQueueTest, enqueue_works_on_an_empty_queue) {
  clock_wait_queue q;
  clock_wait_queue_init(&q);

  clock_wait cw;
  cw.tid = 5;
  cw.ticks = 789;

  clock_wait_queue_enqueue(&q, &cw);

  ASSERT_EQ(1, q.size);
  ASSERT_EQ(5, q.elts[0].tid);
  ASSERT_EQ(789, q.elts[0].ticks);
}

TEST(ClockWaitQueueTest, enqueue_maintains_heap_invariant) {
  clock_wait_queue q;
  clock_wait_queue_init(&q);
  populate_queue(&q);

  int heaped_tids[CLOCK_WAIT_QUEUE_SIZE] = { 8, 5, 7, 4, 2, 6, 3, 1 };
  uint32_t heaped_ticks[CLOCK_WAIT_QUEUE_SIZE] = { 10, 23, 232, 111, 231, 1200, 789, 555 };

  ASSERT_EQ(CLOCK_WAIT_QUEUE_SIZE, q.size);
  for (int i = 0; i < CLOCK_WAIT_QUEUE_SIZE; i += 1) {
    ASSERT_EQ(heaped_tids[i], q.elts[i].tid);
    ASSERT_EQ(heaped_ticks[i], q.elts[i].ticks);
  }
}

TEST(ClockWaitQueueTest, enqueue_returns_negative_one_if_the_queue_is_full) {
  clock_wait_queue q;
  clock_wait_queue_init(&q);

  clock_wait cw;
  cw.tid = 5;
  cw.ticks = 100;

  for (int i = 0; i < CLOCK_WAIT_QUEUE_SIZE; i += 1) {
    ASSERT_EQ(0, clock_wait_queue_enqueue(&q, &cw));
    ASSERT_EQ(i + 1, q.size);
  }

  ASSERT_EQ(-1, clock_wait_queue_enqueue(&q, &cw));
  ASSERT_EQ(CLOCK_WAIT_QUEUE_SIZE, q.size);
}

TEST(ClockWaitQueueTest, dequeue_removes_element_from_heap) {
  clock_wait_queue q;
  clock_wait_queue_init(&q);
  populate_queue(&q);

  clock_wait cw;

  clock_wait_queue_dequeue(&q, &cw);
  ASSERT_EQ(7, q.size);
}

TEST(ClockWaitQueueTest, dequeue_removes_elements_in_sorted_order) {
  clock_wait_queue q;
  clock_wait_queue_init(&q);
  populate_queue(&q);

  clock_wait cw;
  int sorted_tids[CLOCK_WAIT_QUEUE_SIZE] = { 8, 5, 4, 2, 7, 1, 3, 6 };
  uint32_t sorted_ticks[CLOCK_WAIT_QUEUE_SIZE] = { 10, 23, 111, 231, 232, 555, 789, 1200 };

  for (int i = 0; i < CLOCK_WAIT_QUEUE_SIZE; i += 1) {
    clock_wait_queue_dequeue(&q, &cw);
    ASSERT_EQ(sorted_tids[i], cw.tid);
    ASSERT_EQ(sorted_ticks[i], cw.ticks);
  }
}

TEST(ClockWaitQueueTest, dequeue_returns_negative_one_if_queue_is_empty) {
  clock_wait_queue q;
  clock_wait_queue_init(&q);

  ASSERT_EQ(-1, clock_wait_queue_dequeue(&q, nullptr));
}

TEST(ClockWaitQueueTest, peek_returns_a_pointer_to_the_first_element) {
  clock_wait_queue q;
  clock_wait_queue_init(&q);

  clock_wait cw;

  for (int i = 0; i < CLOCK_WAIT_QUEUE_SIZE; i += 1) {
    cw.tid = i + 1;
    cw.ticks = 500 - i * 100;
    clock_wait_queue_enqueue(&q, &cw);
  }

  clock_wait *result = clock_wait_queue_peek(&q);
  ASSERT_EQ(result, &(q.elts[0]));
}

TEST(ClockWaitQueueTest, peek_returns_null_clock_wait_if_queue_is_empty) {
  clock_wait_queue q;
  clock_wait_queue_init(&q);

  ASSERT_EQ(NULL_CLOCK_WAIT, clock_wait_queue_peek(&q));
}

#ifndef ALLTESTS
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#endif /* ALLTESTS */
