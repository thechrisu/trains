#if VERSATILEPB
#include "cp_vec.h"
#endif

#include "crash.h"
#include "events.h"
#include "interrupt.h"
#include "k3.h"
#include "myio.h"
#include "mytimer.h"
#include "schedule.h"
#include "send_queue.h"
#include "stdlib.h"
#include "syscall.h"
#include "task.h"
#include "test_runner.h"
#include "usage_stats.h"
#include "kusage_stats.h"
#include "vic.h"

#ifndef VERSATILEPB
#include "kernel/include/labenv/ts7200.h"
extern void enter_kernel(unsigned int syscall_code);
extern void handle_data_abort();
extern void handle_prefetch_abort();
extern void handle_undefined_abort();
#endif /* VERSATILEPB */
extern uint64_t available_tids;
extern int the_next_generation[MAX_TASKS];

trapframe main_tf;

extern int ticks;
extern int num_ctx_sw;

void kmain() {
  setup_io();

  log_index = 0;

  // Initialize tick count
  ticks = 0;

  num_ctx_sw = 0;
  for (int i = 0; i < NUM_SYSCALL_CODES; i += 1) {
    num_syscalls[i] = 0;
  }
  num_syscalls_total = 0;

  setup_vic();

#if !E2ETESTING || TIMER_INTERRUPTS
  // Setup tick timer
  interrupt_timer_setup();
#endif /* !E2ETESTING || TIMER_INTERRUPTS */

#if !E2ETESTING || IOINTERRUPTS
  setup_iio();
#endif /* !E2ETESTING || IOINTERRUPTS */

  available_tids = 0x7FFFFFFFFFFFFFFF;
  for (int i = 0; i < MAX_TASKS; i += 1)
    the_next_generation[i] = 1;

#pragma GCC diagnostic ignored "-Wformat-zero-length"
  logprintf("");
#pragma GCC diagnostic warning "-Wformat-zero-length"

  setup_events();

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

  setup_timer();
  setup_kusage_stats();

  start_interval();
#if E2ETESTING
  syscall_create(1, &test_runner);
#else
  syscall_create(10, &k3_first_user_task);
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
    "STR r0, [%0, #0]\n\t"
    "STR r1, [%0, #4]\n\t"
    "STR r2, [%0, #8]\n\t"
    "STR r3, [%0, #12]\n\t"
    "STR r4, [%0, #16]\n\t"
    "STR r5, [%0, #20]\n\t"
    "STR r6, [%0, #24]\n\t"
    "STR r7, [%0, #28]\n\t"
    "STR r8, [%0, #32]\n\t"
    "STR r9, [%0, #36]\n\t"
    "STR r10, [%0, #40]\n\t"
    "STR r11, [%0, #44]\n\t"
    "STR r12, [%0, #48]\n\t"
    "STR r13, [%0, #52]\n\t"
    "STR r14, [%0, #56]\n\t"
  : : "r" (&main_tf));

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

  __asm__(
    "LDR r0, [%0, #0]\n\t"
    "LDR r1, [%0, #4]\n\t"
    "LDR r2, [%0, #8]\n\t"
    "LDR r3, [%0, #12]\n\t"
    "LDR r4, [%0, #16]\n\t"
    "LDR r5, [%0, #20]\n\t"
    "LDR r6, [%0, #24]\n\t"
    "LDR r7, [%0, #28]\n\t"
    "LDR r8, [%0, #32]\n\t"
    "LDR r9, [%0, #36]\n\t"
    "LDR r10, [%0, #40]\n\t"
    "LDR r11, [%0, #44]\n\t"
    "LDR r12, [%0, #48]\n\t"
    "LDR r13, [%0, #52]\n\t"
    "LDR r14, [%0, #56]\n\t"
  : : "r" (&main_tf));

  // Disable VIC
  vic_maskall();
  *(uint32_t *)(VIC1_BASE + VIC1_ENABLE_OFFSET) = 0x0;
  *(uint32_t *)(VIC2_BASE + VIC2_ENABLE_OFFSET) = 0x0;


#if !E2ETESTING || TIMER_INTERRUPTS
  interrupt_timer_teardown();

  bwprintf("Total number of context switches: %d\n\r", num_ctx_sw);
  bwprintf("Total number of syscalls: %d\n\r", num_syscalls_total);

  bwprintf("Number of syscalls by code:\n\r");
  bwprintf("Code\tCount\n\r");
  for (int i = 0; i < NUM_SYSCALL_CODES; i += 1) {
    bwprintf("%d\t%d\n\r", i, num_syscalls[i]);
  }

  usage_stats usage;
  syscall_total_proc_usage(&usage);
  print_usage(bwprintf, &usage);
#endif /* E2ETESTING && TIMER_INTERRUPTS */

  desetup_iio();

#if VERSATILEPB
#if E2ETESTING
  bwprintf("ENDPROG\n\r");
#endif /* E2ETESTING */

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
