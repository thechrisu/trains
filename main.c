#if VERSATILEPB
#include "cp_vec.h"
#endif

#include "crash.h"
#include "tasks.h"
#include "interrupt.h"
#include "stdlib.h"
#include "myio.h"
#include "./src/syscall/syscall.h"
#include "./src/multitasking/schedule.h"

#ifndef VERSATILEPB
extern void enter_kernel(unsigned int syscall_code);
#endif
extern int next_task_id;

unsigned int handle_interrupt_fp;
unsigned int handle_interrupt_sp;

void kmain() {
  setup_io();
  task_descriptor all_tasks_on_stack[MAX_TASKS];
  all_tasks = (task_descriptor*)all_tasks_on_stack;
  next_task_id = 1;
#ifndef VERSATILEPB
  uint32_t *swi_handler = (uint32_t *)0x28;
  *swi_handler = (uint32_t)(&enter_kernel);
#endif /* VERSATILEPB */

  setup_scheduler();
#if CONTEXT_SWITCH_DEBUG
  bwprintf("Set up scheduler\n\r");
#endif /* CONTEXT_SWTICH_DEBUG */
  // TODO call syscall_create to create ONLY first user task
  syscall_create(5, &first_user_task);

#if CONTEXT_SWITCH_DEBUG
  bwprintf("Set up tasks\n\r");
#endif /* CONTEXT_SWITCH_DEBUG */  
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
   __asm__(
	"MOV r0, #0x18\n\t"
	"MOV r1, #0x20000\n\t"
	"ADD r1, r1, #0x026\n\t"
    "SVC 0x00123456\n\t"
  ); // ALlows us to quit QEMU cleanly
#else
  return 0;
#endif /* VERSATILEPB */
}
