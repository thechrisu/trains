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
  bwprintf("We made it, again\n\r");
  bwprintf("We made it, again\n\r");
  software_interrupt(0);
  bwprintf("We made it, again\n\r");
  bwprintf("We made it, again\n\r");
  CRASH();
}

void second_user_task() {
  bwprintf("Second user task\n\r");
  software_interrupt(0);
  bwprintf("Second user task, again\n\r");
}
