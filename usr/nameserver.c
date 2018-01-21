#include "nameserver.h"

#define NAMESERVER_MSG_LENGTH 32
#define TOTAL_NUM_NAMES 512

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
          memcpy(names[next_name], incoming_msg_buffer+1, ret - 1); // TODO overwrite old names
          task_ids[next_name] = sender_tid;
          next_name++;
          break;
        case 'W': {
          bool found = false;
          for (int i = 0; i < next_name; i++) {
            if (strncmp(names[i], incoming_msg_buffer + 1, NAMESERVER_MSG_LENGTH - 1) == 0) {
              found = true;
              Reply(sender_tid, (char *)&task_ids[i], 1);
              break;
            }
          }
          if (!found) {
            char not_found_warning =  'N' + 128;
            Reply(sender_tid, &not_found_warning, 1);
          }
          break;
        }
        default: {
          char wrong_command = 'W' + 128; // "Wrong"
          Reply(sender_tid, &wrong_command, 1);
        }
      }
    }
  }
}
