#include "test_mypriority.h"

void second_tester() {
  bwprintf("Task should have priority 20, and is %d\n\r", MyPriority());
}

void test_mypriority() {
  Create(20, &second_tester);
}
