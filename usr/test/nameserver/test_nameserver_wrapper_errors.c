#include "test_nameserver_wrapper_errors.h"

void test_nameserver_wrapper_errors() {
  ns_tid = Create(5, &nameserver_main);

  bwprintf("Truncated RegisterAs: %d\n\r", RegisterAs("This is much too long for the nameserver to handle"));
  bwprintf("Truncated WhoIs: %d\n\r", WhoIs("This is much too long for the nameserver to handle"));

  bwprintf("Too short RegisterAs: %d\n\r", RegisterAs(""));
  bwprintf("Too short WhoIs: %d\n\r", WhoIs(""));

  bwprintf("Not found WhoIs: %d\n\r", WhoIs("Bill Cowan"));

  fake_Kill();
}
