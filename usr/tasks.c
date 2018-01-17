#include "myio.h"
#include "crash.h"

unsigned int software_interrupt(unsigned int num) {
  register int arg0 __asm__ ("r0");
  int result;

  arg0 = num;
  __asm__ volatile (
    "swi 0\n\t"
    "mov %0, r0"
  : "=r" (result) : "r" (arg0));
#ifdef CONTEXT_SWITCH_DEBUG
  bwprintf("End of software_interrupt\n\r");
#endif /* CONTEXT_SWITCH_DEBUG */
  return result;
}

int first_user_task() {
  int i;

  bwprintf("We made it!\n\r");

  i = software_interrupt(0xDEADBEEF);
  bwprintf("We made it, twice\n\r");
  bwprintf("Return value: %x\n\r", i);
  software_interrupt(1);
  bwprintf("We made it, thrice\n\r");
  software_interrupt(1);
  bwprintf("We made it, 4x\n\r");
  software_interrupt(1);
  bwprintf("TASK 1: THIS SHOULD PRINT\n\r");
  return 0xDEADBEEF;
}

void second_user_task() {
  bwprintf("Second user task once\n\r");
  software_interrupt(1);
  bwprintf("Second user task twice\n\r");
  software_interrupt(1);
  bwprintf("Second user task thrice\n\r");
  software_interrupt(1);
  bwprintf("Second user task, finally, now return to main\n\r");
}
