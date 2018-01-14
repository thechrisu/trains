#include "myio.h"
#include "crash.h"

unsigned int software_interrupt(unsigned int num) {
  register int arg0 __asm__ ("r0");
  register int result __asm__ ("r0");

  arg0 = num;
  __asm__("swi 1" : "=r" (result) : "r" (arg0));
  return result;
}

void first_user_task() {
  int i;

  bwprintf("We made it!\n\r");

  i = software_interrupt(0xDEADBEEF);
  bwprintf("We made it, twice\n\r");
  bwprintf("Return value: %x\n\r", i);
  software_interrupt(0);
  bwprintf("We made it, thrice\n\r");
  software_interrupt(0);
  bwprintf("We made it, 4x\n\r");
  software_interrupt(0);
  bwprintf("TASK 1: THIS SHOULD NEVER PRINT\n\r");
  //  int c = 1;
}

void second_user_task() {
  bwprintf("Second user task once\n\r");
  software_interrupt(0);
  bwprintf("Second user task twice\n\r");
  software_interrupt(0);
  bwprintf("Second user task thrice\n\r");
  software_interrupt(0);
#if VERSATILEPB
  bwprintf("Second user task, finally, now crash\n\r");
  CRASH();
#else
  bwprintf("Second user task, finally, now return to main\n\r");
#endif /* VERSTILEPB */
  //  int c = 1;
}
