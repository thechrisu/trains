#include "test_ready_queue.h"
#include <gtest/gtest.h>

using namespace std;

TEST(ReadyQueueTest, length_returns_zero_for_null_queue) {
  ready_queue rq = NULL_READY_QUEUE;
  ASSERT_EQ(ready_queue_length(&rq), 0);
}

TEST(ReadyQueueTest, is_empty_returns_true_for_an_empty_queue) {
  ready_queue rq = NULL_READY_QUEUE;
  ASSERT_TRUE(ready_queue_is_empty(&rq));
}

TEST(ReadyQueueTest, is_empty_returns_false_for_a_non_empty_queue) {
  task_descriptor td;
  ready_queue rq = NULL_READY_QUEUE;

  ready_queue_enqueue(&rq, &td);
  ASSERT_FALSE(ready_queue_is_empty(&rq));
}

TEST(ReadyQueueTest, enqueue_enqueues_a_task_onto_an_empty_queue) {
  task_descriptor td;
  ready_queue rq = NULL_READY_QUEUE;

  ready_queue_enqueue(&rq, &td);

  ASSERT_EQ(ready_queue_length(&rq), 1);
  ASSERT_EQ(rq->next, &td);
  ASSERT_EQ(rq->prev, &td);
}

TEST(ReadyQueueTest, enqueue_enqueues_a_task_onto_a_queue_with_one_task) {
  task_descriptor td1, td2;
  ready_queue rq = NULL_READY_QUEUE;

  ready_queue_enqueue(&rq, &td1);
  ready_queue_enqueue(&rq, &td2);

  ASSERT_EQ(ready_queue_length(&rq), 2);
  ASSERT_EQ(rq->next, &td2);
  ASSERT_EQ(rq->prev, &td2);
  ASSERT_EQ(rq->next->next, &td1);
  ASSERT_EQ(rq->prev->prev, &td1);
}

TEST(ReadyQueueTest, enqueue_enqueues_a_task_onto_a_queue_with_two_tasks) {
  task_descriptor td1, td2, td3;
  ready_queue rq = NULL_READY_QUEUE;

  ready_queue_enqueue(&rq, &td1);
  ready_queue_enqueue(&rq, &td2);
  ready_queue_enqueue(&rq, &td3);

  ASSERT_EQ(ready_queue_length(&rq), 3);
  ASSERT_EQ(rq->next, &td2);
  ASSERT_EQ(rq->prev, &td3);
  ASSERT_EQ(rq->next->next, &td3);
  ASSERT_EQ(rq->prev->prev, &td2);
  ASSERT_EQ(rq->next->next->next, &td1);
  ASSERT_EQ(rq->prev->prev->prev, &td1);
}

TEST(ReadyQueueTest, enqueue_enqueues_a_task_onto_a_queue_with_five_tasks) {
  int i;
  ready_queue rq = NULL_READY_QUEUE;

  task_descriptor td[6];
  for (i = 0; i < 6; i += 1) {
    ready_queue_enqueue(&rq, &(td[i]));
  }

  ASSERT_EQ(ready_queue_length(&rq), 6);
  ASSERT_EQ(rq->next, &(td[1]));
  ASSERT_EQ(rq->prev, &(td[5]));
}

TEST(ReadyQueueTest, dequeue_returns_null_task_descriptor_if_the_queue_is_empty) {
  ready_queue rq = NULL_READY_QUEUE;
  ASSERT_EQ(ready_queue_dequeue(&rq), NULL_TASK_DESCRIPTOR);
  ASSERT_EQ(ready_queue_length(&rq), 0);
}

TEST(ReadyQueueTest, dequeue_dequeues_a_task_from_a_queue_with_one_task) {
  task_descriptor td1;
  ready_queue rq = NULL_READY_QUEUE;

  ready_queue_enqueue(&rq, &td1);

  ASSERT_EQ(ready_queue_dequeue(&rq), &td1);
  ASSERT_EQ(ready_queue_length(&rq), 0);
}

TEST(ReadyQueueTest, dequeue_dequeues_a_task_from_a_queue_with_two_tasks) {
  task_descriptor td1, td2;
  ready_queue rq = NULL_READY_QUEUE;

  ready_queue_enqueue(&rq, &td1);
  ready_queue_enqueue(&rq, &td2);

  ASSERT_EQ(ready_queue_dequeue(&rq), &td1);
  ASSERT_EQ(ready_queue_length(&rq), 1);
}

TEST(ReadyQueueTest, dequeue_dequeues_a_task_from_a_queue_with_five_tasks) {
  int i;
  ready_queue rq = NULL_READY_QUEUE;

  task_descriptor td[5];
  for (i = 0; i < 5; i += 1) {
    ready_queue_enqueue(&rq, &(td[i]));
  }

  ASSERT_EQ(ready_queue_dequeue(&rq), &(td[0]));
  ASSERT_EQ(ready_queue_length(&rq), 4);
}

#ifndef ALLTESTS
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#endif /* ALLTESTS */
