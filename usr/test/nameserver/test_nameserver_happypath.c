#include "test_nameserver_happypath.h"

void other_task() {
  RegisterAs("Task2");
}

void overwriting_task() {
  RegisterAs("Task2");
}

void test_nameserver_happypath() {
  ns_tid = Create(MyPriority(), &nameserver_main);

  RegisterAs("Task1");
  bwprintf("Basic RegisterAs/WhoIs: Me: %d, Task in question: %d\n\r", MyTid(), WhoIs("Task1"));

  int other_tid = Create(MyPriority() + 1, &other_task);
  bwprintf("Other task registers: Them: %d, Task registered: %d\n\r", other_tid, WhoIs("Task2"));

  int overwriting_tid = Create(MyPriority() + 1, &overwriting_task);
  bwprintf("Overwriting task: Them: %d, Task registered: %d\n\r", overwriting_tid, WhoIs("Task2"));

  RegisterAs("Task3");
  bwprintf("I'm %d and registered Task1 and Task3. Nameserver says Task1 and Task3 are %d and %d\n\r",
           MyTid(), WhoIs("Task1"), WhoIs("Task3"));

  fake_Kill();
}
