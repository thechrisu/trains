#include "../include/common/codes.h"
#include "myio.h"

int first_user_task() {
  int i;

  bwprintf("We made it!\n\r");
  Pass();
  bwprintf("We made it, twice\n\r");
  bwprintf("MyTid: %x\n\r", MyTid());
  Pass();
  bwprintf("We made it, thrice\n\r");
  Pass();
  bwprintf("We made it, 4x\n\r");
  Pass();
  bwprintf("TASK 1: THIS SHOULD PRINT\n\r");
  return 0xDEADBEEF;
}

void second_user_task() {
  bwprintf("Second user task once\n\r");
  Pass();
  bwprintf("Second user task twice\n\r");
  Pass();
  bwprintf("Second user task thrice\n\r");
  Pass();
  bwprintf("Second user task, finally, now return to main\n\r");
}
