#include "fake_nameserver_functions.h"

int ns_tid;

char send_msg(char *c, char msg_type) {
  char reply;
  char msg[tstrlen(c) + 2];
  msg[0] = msg_type;
  tmemcpy(msg + 1, c, tstrlen(c) + 1);
  Assert(Send(ns_tid, msg, tstrlen(c) + 2, &reply, 1) > -1);
  return reply;
}

char fake_RegisterAs(char *c) {
  return send_msg(c, 'R');
}

char fake_WhoIs(char *c) {
  return send_msg(c, 'W');
}

char fake_Kill(){
  return send_msg("", 'K');
}
