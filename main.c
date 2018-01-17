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

unsigned int main_fp;
unsigned int main_sp;

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


*/
/**
 * A wrapper for <code>kmain()</code> that sets up the syscall_abort function,
 * which is used by:
 *   - <code>kassert()</code>
 *   - The <code>Abort()</code> syscall
 *
 * @returns 0 on the ARM box, crashes without returning in QEMU.
 */
int main() {
  __asm__( /* CALLS TO KASSERT ABOVE THIS LINE MAY CAUSE BUGS */
    "mov %0, fp\n\t"
    "mov %1, sp\n\t"
  : "=r" (main_fp), "=r" (main_sp));

  /* kmain() contains actual program functionality. */
  kmain();

  /* Calls to syscall_abort branch to this label. */
  __asm__(
    ".text\n\t"
    ".global abort_exit\n\t"
    "abort_exit:\n\t"
  ); /* CALLS TO KASSERT BELOW THIS LINE MAY CAUSE BUGS */

#if VERSATILEPB
  CRASH();
#else
  return 0;
#endif /* VERSATILEPB */
}
