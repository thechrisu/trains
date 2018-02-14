#include "messaging.h"

void transmit_message(task_descriptor *src, task_descriptor *dst) {
#if CONTEXT_SWITCH_BENCHMARK
  volatile int16_t *loc_before_copy = LOC_BEFORE_COPY;
  *loc_before_copy = get_clockticks();
  // logprintf("(%d) Before copy\n\r", *loc_before_copy);
#endif /* CONTEXT_SWITCH_BENCHMARK */

  bool sender_msg_too_big = src->tf->r3 > dst->tf->r3;
  if (sender_msg_too_big) { // Truncated
    dst->tf->r0 = -1;
    tmemcpy((char *)dst->tf->r2, (char *)src->tf->r2, dst->tf->r3);
  } else {
    dst->tf->r0 = src->tf->r3;
    tmemcpy((char *)dst->tf->r2, (char *)src->tf->r2, src->tf->r3);
  }
  task_set_state(src, TASK_REPLY_BLOCKED);
  task_set_state(dst, TASK_RUNNABLE);
  volatile int* sender_tid = (int*)dst->tf->r1;
  *sender_tid = task_get_userland_tid(src);
#if CONTEXT_SWITCH_BENCHMARK
  volatile int16_t *loc_after_copy = LOC_AFTER_COPY;
  *loc_after_copy = get_clockticks();
  // logprintf("(%d) After copy\n\r", *loc_after_copy);
#endif /* CONTEXT_SWITCH_BENCHMARK */
}

void send(task_descriptor *sender, task_descriptor *receiver) {
  switch (receiver->state) {
    case TASK_ZOMBIE:
      sender->tf->r0 = -2;
      break;
    case TASK_SEND_BLOCKED:
      transmit_message(sender, receiver);
      register_task(receiver);
      break;
    default:
      task_set_state(sender, TASK_RECEIVE_BLOCKED);
      send_queue_enqueue(receiver->send_queue, sender);
      break;
  }
}

void receive(task_descriptor *receiver) {
  if(likely(send_queue_is_empty(receiver->send_queue))) {
    task_set_state(receiver, TASK_SEND_BLOCKED);
  } else {
    task_descriptor *sender = send_queue_dequeue(receiver->send_queue);
    transmit_message(sender, receiver);
    task_set_state(sender, TASK_REPLY_BLOCKED);
  }
}

void reply(task_descriptor *called_send, task_descriptor *called_reply) {
  if (called_send->state == TASK_ZOMBIE) {
    called_reply->tf->r0 = -2;
  } else if (called_send->state != TASK_REPLY_BLOCKED) {
    called_reply->tf->r0 = -3;
  } else {
    if (called_send->tf->r5 < called_reply->tf->r3) { // is truncated?
      called_send->tf->r0 = -1;
      called_reply->tf->r0 = -1;
      tmemcpy((char *)called_send->tf->r4, (char *)called_reply->tf->r2, called_send->tf->r5);
    } else {
      called_send->tf->r0 = called_reply->tf->r3;
      called_reply->tf->r0 = 0;
      tmemcpy((char *)called_send->tf->r4, (char *)called_reply->tf->r2, called_reply->tf->r3);
    }
    task_set_state(called_send, TASK_RUNNABLE);
    register_task(called_send);
  }
}
