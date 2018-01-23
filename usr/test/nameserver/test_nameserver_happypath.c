#include "test_nameserver_happypath.h"

void other_task() {
  fake_RegisterAs("Task2");
}

void overwriting_task() {
  fake_RegisterAs("Task2");
}

void test_nameserver_happypath() {
  ns_tid = Create(5, &nameserver_main);

  fake_RegisterAs("Task1");
  bwprintf("Basic RegisterAs/WhoIs: Me: %d, Task in question: %d\n\r", MyTid(), (int)fake_WhoIs("Task1"));

  int other_tid = Create(6, &other_task);
  bwprintf("Other task registers: Them: %d, Task registered: %d\n\r", other_tid, (int)fake_WhoIs("Task2"));

  int overwriting_tid = Create(6, &overwriting_task);
  bwprintf("Overwriting task: Them: %d, Task registered: %d\n\r", overwriting_tid, (int)fake_WhoIs("Task2"));

  fake_RegisterAs("Task3");
  bwprintf("I'm %d and registered Task1 and Task3. Nameserver says Task1 and Task3 are %d and %d\n\r",
           MyTid(), (int)fake_WhoIs("Task1"), (int)fake_WhoIs("Task3"));

  fake_Kill();
}
