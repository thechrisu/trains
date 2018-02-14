#include "nameserver.h"

int get_index_of_name(char *name, char names[TOTAL_NUM_NAMES][NAMESERVER_MSG_LENGTH], int next_name) {
  for (int i = 0; i < next_name; i++) {
    if (tstrncmp(names[i], name, NAMESERVER_MSG_LENGTH - 1) == 0) {
      return i;
    }
  }
  return -1;
}

void nameserver_main() {
  char names[TOTAL_NUM_NAMES][NAMESERVER_MSG_LENGTH];
  int task_ids[TOTAL_NUM_NAMES];
  int next_name = 0;

  message reply;
  reply.type = REPLY_NAMESERVER;

  while (true) {
    int sender_tid;
    char incoming_msg_buffer[NAMESERVER_MSG_LENGTH];
    int ret = Receive(&sender_tid, incoming_msg_buffer, NAMESERVER_MSG_LENGTH);
    if (ret == -1) {
      reply.msg.nameserver_response = -2;
    } else if (ret <= 2) {
      reply.msg.nameserver_response = -3;
    } else {
      switch (incoming_msg_buffer[0]) {
        case 'R':
          if (next_name >= TOTAL_NUM_NAMES) {
            reply.msg.nameserver_response = -4;
          } else {
            int index = get_index_of_name(incoming_msg_buffer + 1, names, next_name);
            if (index == -1) {
              task_ids[next_name] = sender_tid;
              tmemcpy(names[next_name], incoming_msg_buffer + 1, ret - 1);
              next_name++;
            } else {
              task_ids[index] = sender_tid;
            }
            reply.msg.nameserver_response = 0;
          }
          break;
        case 'W': {
          int index = get_index_of_name(incoming_msg_buffer + 1, names, next_name);
          reply.msg.nameserver_response = index == -1 ? -4 : task_ids[index];
          break;
        }
        case 'K': {
          return;
        }
        default: {
          reply.msg.nameserver_response = 0xABAD10DE;
        }
      }
    }

    Reply(sender_tid, &reply, sizeof(reply)); // including \0
  }
}
