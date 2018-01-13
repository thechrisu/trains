#include "myio.h"
#include "crash.h"

int count = 0;

void first_user_task() {
  // bwprintf("We made it!\n\r");

  if (count == 5) {
    CRASH();
  }
  count += 1;

  __asm__("swi 0");
}

void second_user_task() {
  // bwprintf("Second user task\n\r");
  __asm__("swi 0");
}
