#include "test_nameserver_errors.h"


void test_nameserver_errors() {
  ns_tid = Create(5, &nameserver_main);

  char send_buf[NAMESERVER_MSG_LENGTH];
  char reply_buf;

  Send(ns_tid, send_buf, NAMESERVER_MSG_LENGTH + 1, &reply_buf, 1);
  bwprintf("Truncated: %d\n\r", reply_buf);

  Send(ns_tid, send_buf, 0, &reply_buf, 1);
  bwprintf("Too short(0): %d\n\r", reply_buf);

  Send(ns_tid, send_buf, 1, &reply_buf, 1);
  bwprintf("Too short(1): %d\n\r", reply_buf);

  send_buf[0] = 'X';
  Send(ns_tid, send_buf, 2, &reply_buf, 1);
  bwprintf("Wrong: %d\n\r", reply_buf);

  bwprintf("Not found: %d\n\r", fake_WhoIs("Bill Cowan"));

  fake_Kill();
}
