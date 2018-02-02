#include "test_mypriority.h"

void second_tester() {
  bwprintf("Task should have priority MAX_PRIORITY(%d), and is %d\n\r", MAX_PRIORITY, MyPriority());
}

void test_mypriority() {
  Create(MAX_PRIORITY, &second_tester);
}
