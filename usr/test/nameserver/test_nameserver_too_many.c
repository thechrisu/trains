#include "test_nameserver_too_many.h"

void test_nameserver_too_many() {
  ns_tid = Create(5, &nameserver_main);

  for(int i = 0; i < 63; i++) {
    fake_RegisterAs("SPAM");
  }
  bwprintf("fake_RegisterAs before too many: %d\n\r", fake_RegisterAs("The one before"));
  bwprintf("fake_RegisterAs one too many: %d\n\r", fake_RegisterAs("Culprit"));

  fake_Kill();
}
