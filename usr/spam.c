#include "spam.h"

void spam_client() {
  int tid = WhoIs("Spam");
  int my_tid = MyTid();
  char message[] = "ABC";
  char reply[4];

  for (int i = 0; i < 1000; i += 1) {
    Send(tid, message, 4, reply, 4);
    bwprintf("Task %d on loop %d got %s\n\r", my_tid, i, reply);
  }
}

void spam_server() {
  int tid;
  char receive[4];
  char reply[] = "DEF";

  RegisterAs("Spam");

  while (true) {
    Receive(&tid, receive, 4);
    Reply(tid, reply, 4);
  }
}

void useless_fct(int i) {
  if (i < 3) {
    useless_fct(i + 1);
  } else {
    Pass();
  }
}

void spam() {
  Create(MyPriority() - 1, &nameserver_main);
  Create(MyPriority() - 1, &spam_server);
  for (int i = 0; i < 20; i += 1) {
    Create(MyPriority() - 2, &spam_client);
  }
}
