#if VERSATILEPB
#include "cp_vec.h"
#endif

#include "crash.h"
#include "interrupt.h"
#include "k2.h"
#include "myio.h"
#include "mytimer.h"
#include "schedule.h"
#include "send_queue.h"
#include "stdlib.h"
#include "syscall.h"
#include "task.h"
#include "test_runner.h"

#ifndef VERSATILEPB
extern void enter_kernel(unsigned int syscall_code);
extern void handle_data_abort();
extern void handle_prefetch_abort();
extern void handle_undefined_abort();
#endif /* VERSATILEPB */
extern int next_task_id;

unsigned int main_fp;
unsigned int main_sp;

extern int ticks;
extern int num_ctx_sw;

void kmain() {
  setup_io();

  log_index = 0;

  // Initialize tick count
  ticks = 0;

  num_ctx_sw = 0;

  // Setup PIC
  *(uint32_t *)(VIC_BASE + VIC_ENABLE_OFFSET) = VIC_TIMER_MASK;

  interrupt_timer_setup();

  // Setup tick timer
#if VERSATILEPB
#else
#endif /* VERSATILEPB */

  next_task_id = 1;

#pragma GCC diagnostic ignored "-Wformat-zero-length"
  logprintf("");
#pragma GCC diagnostic warning "-Wformat-zero-length"

#ifndef VERSATILEPB
  uint32_t *undefined_handler = (uint32_t*)0x24;
  uint32_t *swi_handler = (uint32_t *)0x28;
  uint32_t *prefetch_handler = (uint32_t*)0x2C;
  uint32_t *data_handler = (uint32_t*)0x30;
  uint32_t *irq_handler = (uint32_t*)0x38;
  *undefined_handler = (uint32_t)(&handle_undefined_abort);
  *swi_handler = (uint32_t)(&enter_kernel);
  *prefetch_handler = (uint32_t)(&handle_prefetch_abort);
  *data_handler = (uint32_t)(&handle_data_abort);
  *irq_handler = (uint32_t)(&enter_kernel);
#endif /* VERSATILEPB */

  setup_scheduler();
#if CONTEXT_SWITCH_DEBUG
  logprintf("Set up scheduler\n\r");
#endif /* CONTEXT_SWTICH_DEBUG */

#if E2ETESTING
  syscall_create(1, &test_runner);
#else
  syscall_create(10, &k2_first_user_task);
#endif /* TESTING */

#if CONTEXT_SWITCH_DEBUG
  logprintf("Set up tasks\n\r");
#endif /* CONTEXT_SWITCH_DEBUG */
  while(schedule());

#if CONTEXT_SWITCH_DEBUG
  logprintf("Return from schedule\n\r");
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

  __asm__(
    "msr cpsr_c, #0xD2\n\t"   // Enter IRQ mode
    "mov sp, #0xDA000000\n\t" // Put magic value in sp_irq
    "msr cpsr_c, #0xD3\n\t"   // Go back to kernel mode
  );

  // Check that interrupts are disabled and we're in kernel mode.
  register_t psr;
  __asm__("MRS %0, cpsr" : "=r" (psr));
  kassert((psr & 0xFF) == 0xD3);

  /* kmain() contains actual program functionality. */
  kmain();

#ifndef E2ETESTING
  logprintf("Ticks: %d\n\r", ticks);
#endif

  dump_logs();

  /* Calls to syscall_panic branch to this label. */
  __asm__(
    ".global panic_exit\n\t"
    "panic_exit:\n\t"
  ); /* CALLS TO KASSERT BELOW THIS LINE MAY CAUSE BUGS */

  // Disable VIC
  *(uint32_t *)(VIC_BASE + VIC_ENABLE_OFFSET) = 0x0;

  interrupt_timer_teardown();

#if TIMERINTERRUPT_DEBUG
  bwprintf("Total number of context switches: %d\n\r", num_ctx_sw);
#endif /* TIMERINTERRUPT_DEBUG */

#if VERSATILEPB
  __asm__(
    "MOV r0, #0x18\n\t"
    "MOV r1, #0x20000\n\t"
    "ADD r1, r1, #0x026\n\t"
    "SWI 0x00123456\n\t"
  ); // Allows us to quit QEMU cleanly
#else
  return 0;
#endif /* VERSATILEPB */
}
