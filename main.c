#if VERSATILEPB
#include "cp_vec.h"
#endif

#include "crash.h"
#include "k1.h"
#include "test/test_runner.h"
#include "interrupt.h"
#include "stdlib.h"
#include "myio.h"
#include "./src/syscall/syscall.h"
#include "./src/multitasking/schedule.h"
#include "./src/multitasking/send_queue.h"
#include "./src/multitasking/task.h"

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

  send_queue send_queues_on_stack[MAX_TASKS];
  send_queues = send_queues_on_stack;

  next_task_id = 1;
#ifndef VERSATILEPB
  uint32_t *swi_handler = (uint32_t *)0x28;
  *swi_handler = (uint32_t)(&enter_kernel);
#endif /* VERSATILEPB */

  setup_scheduler();
#if CONTEXT_SWITCH_DEBUG
  bwprintf("Set up scheduler\n\r");
#endif /* CONTEXT_SWTICH_DEBUG */

#if E2ETESTING
  syscall_create(1, &test_runner);
#else
  syscall_create(5, &k1_first_user_task);
#endif /* TESTING */

#if CONTEXT_SWITCH_DEBUG
  bwprintf("Set up tasks\n\r");
#endif /* CONTEXT_SWITCH_DEBUG */
  while(schedule());

#if CONTEXT_SWITCH_DEBUG
  bwprintf("Return from schedule\n\r");
#endif /* CONTEXT_SWITCH_DEBUG */
}

/**
 * A wrapper for <code>kmain()</code> that sets up the syscall_panic function,
 * which is used by:
 *   - <code>kassert()</code>
 *   - The <code>Panic()</code> syscall
 *   - The <code>Assert()</code> syscall
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

  /* Calls to syscall_panic branch to this label. */
  __asm__(
    ".text\n\t"
    ".global panic_exit\n\t"
    "panic_exit:\n\t"
  ); /* CALLS TO KASSERT BELOW THIS LINE MAY CAUSE BUGS */

#if VERSATILEPB
   __asm__(
	"MOV r0, #0x18\n\t"
	"MOV r1, #0x20000\n\t"
	"ADD r1, r1, #0x026\n\t"
    "SVC 0x00123456\n\t"
  ); // Allows us to quit QEMU cleanly
#else
  return 0;
#endif /* VERSATILEPB */
}
