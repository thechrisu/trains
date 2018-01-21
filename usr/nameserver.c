#include "nameserver.h"

#define NAMESERVER_MSG_LENGTH 32

void nameserver_main() {
  int sender_tid;
  char incoming_msg_buffer[NAMESERVER_MSG_LENGTH];
  int ret = Receive(&sender_tid, incoming_msg_buffer, NAMESERVER_MSG_LENGTH);
  if (ret == -1) {
    Reply(sender_tid, )
  }
  switch ()
}
