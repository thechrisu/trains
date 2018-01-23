#include "fake_nameserver_functions.h"

int ns_tid;

char send_msg(char *c, char msg_type) {
  char reply;
  char msg[strlen(c) + 2];
  msg[0] = msg_type;
  memcpy(msg + 1, c, strlen(c) + 1);
  Assert(Send(ns_tid, msg, strlen(c) + 2, &reply, 1) > -1);
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
