#include "myio.h"
#include "crash.h"

int count = 0;

unsigned int software_interrupt(unsigned int num) {
  (void)num;
  __asm__("swi 0");
  bwprintf("Return from swi\n\r");
}

void first_user_task() {
  bwprintf("We made it!\n\r");

  if (count == 5) {
    CRASH();
  }
  count += 1;
  software_interrupt(0);
  bwprintf("We made it, twice\n\r");
  software_interrupt(0);
  bwprintf("We made it, thrice\n\r");
  software_interrupt(0);
  bwprintf("We made it, 4x\n\r");
  software_interrupt(0);
  bwprintf("TASK 1: THIS SHOULD NEVER PRINT\n\r");
}

void second_user_task() {
  bwprintf("Second user task once\n\r");
  software_interrupt(0);
  bwprintf("Second user task twice\n\r");
  software_interrupt(0);
  bwprintf("Second user task thrice\n\r");
  software_interrupt(0);
  bwprintf("Second user task, finally, now crash\n\r");
  CRASH();
}
