#if VERSATILEPB
#include "cp_vec.h"
#endif

#include "crash.h"
#include "tasks.h"
#include "interrupt.h"
#include "stdlib.h"
#include "myio.h"

#ifndef VERSATILEPB
extern void enter_kernel(unsigned int syscall_code);
#endif

unsigned int handle_interrupt_fp;
unsigned int handle_interrupt_sp;

extern unsigned int *stack_pointers;
extern unsigned int current_task;
extern unsigned int tasks_ended;
extern trapframe *main_trapframe;
extern void sys_exit();
extern void leave_main(int ret_code, trapframe *tf);

void kmain() {
  setup_io();

#ifndef VERSATILEPB
  uint32_t *swi_handler = (uint32_t *)0x28;
  *swi_handler = (uint32_t)(&enter_kernel);
#endif /* VERSATILEPB */

  // TODO call syscall_create to create first user task

  while(schedule());

#if CONTEXT_SWITCH_DEBUG
  bwprintf("Return from schedule\n\r");
#endif /* CONTEXT_SWITCH_DEBUG */
}

/*
  A wrapper for kmain() that sets up kernel assertions.
*/
int main() {
  /* Setup variables for kernel assertions */
  __asm__( /* CALLS TO KASSERT ABOVE THIS LINE MAY CAUSE BUGS */
    "mov %0, fp\n\t"
    "mov %1, sp\n\t"
  : "=r" (handle_interrupt_fp), "=r" (handle_interrupt_sp));

  /* kmain() contains actual program functionality. */
  kmain();

  /* Failed kernel assertions branch to this label. */
  __asm__(
    ".text\n\t"
    ".global kassert_exit\n\t"
    "kassert_exit:\n\t"
  ); /* CALLS TO KASSERT BELOW THIS LINE MAY CAUSE BUGS */

#if VERSATILEPB
  CRASH();
#else
  return 0;
#endif /* VERSATILEPB */
}
