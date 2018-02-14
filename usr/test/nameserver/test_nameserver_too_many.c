#include "test_nameserver_too_many.h"

void test_nameserver_too_many() {
  ns_tid = Create(MyPriority(), &nameserver_main);

  for(int i = 1; i < TOTAL_NUM_NAMES; i++) {
    char fake[6];
    fake[0] = i;
    fake[1] = 'S';
    fake[2] = 'P';
    fake[3] = 'A';
    fake[4] = 'M';
    RegisterAs(fake);
  }
  bwprintf("RegisterAs before too many: %d\n\r", RegisterAs("The one before"));
  bwprintf("RegisterAs one too many: %d\n\r", RegisterAs("Culprit"));

  Kill(ns_tid);
}
