#include "block.h"


void send(task_descriptor *sender, task_descriptor *receiver) {
  bool sender_msg_too_big = sender->tf->r3 > receiver->tf->r3;
  switch (receiver->state) {
    case TASK_ZOMBIE:
      sender->tf->r0 = -3;
      break;
    case TASK_RECEIVE_BLOCKED:
      if (sender_msg_too_big) { // Truncated
        receiver->tf->r0 = -1;
        memcpy((void*)receiver->tf->r2, (void*)sender->tf->r2, receiver->tf->r3);
      } else {
        receiver->tf->r0 = sender->tf->r3;
        memcpy((void*)receiver->tf->r2, (void*)sender->tf->r2, sender->tf->r3);
      }
      task_set_state(sender, TASK_REPLY_BLOCKED);
      task_set_state(receiver, TASK_RUNNABLE);
      register_task(receiver);
      break;
    default:
      task_set_state(sender, TASK_SEND_BLOCKED);
      // add it to the send queue for the receiver
      break;
  }
}
