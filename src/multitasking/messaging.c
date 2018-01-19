#include "messaging.h"

void transmit_message(task_descriptor *src, task_descriptor *dst) {
  bool sender_msg_too_big = src->tf->r3 > dst->tf->r3;
  if (sender_msg_too_big) { // Truncated
    dst->tf->r0 = -1;
    memcpy((void*)dst->tf->r2, (void*)src->tf->r2, dst->tf->r3);
  } else {
    dst->tf->r0 = src->tf->r3;
    memcpy((void*)dst->tf->r2, (void*)src->tf->r2, src->tf->r3);
  }
  task_set_state(src, TASK_REPLY_BLOCKED);
  task_set_state(dst, TASK_RUNNABLE);
}

void send(task_descriptor *sender, task_descriptor *receiver) {
  switch (receiver->state) {
    case TASK_ZOMBIE:
      sender->tf->r0 = -3;
      break;
    case TASK_RECEIVE_BLOCKED:
      transmit_message(sender, receiver);
      register_task(receiver);
      break;
    default:
      task_set_state(sender, TASK_SEND_BLOCKED);
      send_queue_enqueue(receiver->send_queue, sender);
      break;
  }
}

void receive(task_descriptor *receiver) {
  if(likely(send_queue_is_empty(receiver->send_queue))) {
    task_set_state(receiver, TASK_RECEIVE_BLOCKED);
  } else {
    task_descriptor *sender = send_queue_dequeue(receiver->send_queue);
    transmit_message(sender, receiver);
  }
}

void reply(task_descriptor *called_send, task_descriptor *called_reply) {
  if (called_send->state != TASK_REPLY_BLOCKED) {
    called_reply->tf->r0 = -3;
  } else {
    if (called_send->tf->r5 < called_reply->tf->r3) { // is truncated?
      called_send->tf->r0 = -1;
      called_reply->tf->r0 = -1;
      memcpy((void*)called_send->tf->r4, (void*)called_reply->tf->r3, called_send->tf->r5);
    } else {
      called_send->tf->r0 = called_reply->tf->r3;
      called_reply->tf->r0 = 0;
      memcpy((void*)called_send->tf->r4, (void*)called_reply->tf->r3, called_reply->tf->r3);
      task_set_state(called_send, TASK_RUNNABLE);
      task_set_state(called_reply, TASK_RUNNABLE);
      register_task(called_send);
    }
  }
}