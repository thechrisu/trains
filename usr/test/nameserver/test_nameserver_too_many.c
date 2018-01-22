#include "test_nameserver_too_many.h"

void test_nameserver_too_many() {
  ns_tid = Create(5, &nameserver_main);

  for(int i = 0; i < TOTAL_NUM_NAMES - 1; i++) {
    char fake[6];
    fake[0] = i;
    fake[1] = 'S';
    fake[2] = 'P';
    fake[3] = 'A';
    fake[4] = 'M';
    fake_RegisterAs(fake);
  }
  bwprintf("fake_RegisterAs before too many: %d\n\r", fake_RegisterAs("The one before"));
  bwprintf("fake_RegisterAs one too many: %d\n\r", fake_RegisterAs("Culprit"));

  fake_Kill();
}
