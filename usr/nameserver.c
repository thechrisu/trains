#include "nameserver.h"

int get_index_of_name(char *name, char names[TOTAL_NUM_NAMES][NAMESERVER_MSG_LENGTH], int next_name) {
  for (int i = 0; i < next_name; i++) {
    if (strncmp(names[i], name, NAMESERVER_MSG_LENGTH - 1) == 0) {
      return i;
    }
  }
  return -1;
}

void nameserver_main() {
  char names[TOTAL_NUM_NAMES][NAMESERVER_MSG_LENGTH];
  int task_ids[TOTAL_NUM_NAMES];
  int next_name = 0;

  while (true) {
    int sender_tid;
    char incoming_msg_buffer[NAMESERVER_MSG_LENGTH];
    int ret = Receive(&sender_tid, incoming_msg_buffer, NAMESERVER_MSG_LENGTH);
    if (ret == -1) {
      char msg_truncated_warning = 'T' + 128; // "T stands for truncated"
      Reply(sender_tid, &msg_truncated_warning, 1); // including \0
    } else if (ret < 2) {
      char too_short_warning = 'S' + 128; // "Short"
      Reply(sender_tid, &too_short_warning, 1);
    } else {
      switch (incoming_msg_buffer[0]) {
        case 'R':
          if (next_name >= TOTAL_NUM_NAMES) {
            char too_many_warning = 'M' + 128; // "Many"
            Reply(sender_tid, &too_many_warning, 1);
          } else {
            int index = get_index_of_name(incoming_msg_buffer + 1, names, next_name);
            if (index == -1) {
              task_ids[next_name] = sender_tid;
              memcpy(names[next_name], incoming_msg_buffer + 1, ret - 1); // TODO overwrite old names
              next_name++;
            } else {
              task_ids[index] = sender_tid;
            }
            char correct = 'C';
            Reply(sender_tid, &correct, 1);
          }
          break;
        case 'W': {
          int index = get_index_of_name(incoming_msg_buffer + 1, names, next_name);
          if (index != -1) {
            Reply(sender_tid, (char *)&task_ids[index], 1);
          } else {
            char not_found_warning =  'N' + 128; // "Not found"
            Reply(sender_tid, &not_found_warning, 1);
          }
          break;
        }
        case 'K': {
          return;
        }
        default: {
          char wrong_command = 'W' + 128; // "Wrong"
          Reply(sender_tid, &wrong_command, 1);
        }
      }
    }
  }
}
