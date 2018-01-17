#include "./syscall/codes.h"
#include "./syscall/syscall.h"
#include "crash.h"
#include "interrupt.h"
#include "myio.h"
#include "tasks.h"

void print_tf(trapframe *tf) {
  bwputr(TERMINAL, tf->r0);
  bwputr(TERMINAL, tf->r1);
  bwputr(TERMINAL, tf->r2);
  bwputr(TERMINAL, tf->r3);
  putc(TERMINAL, '\n');
  putc(TERMINAL, '\r');
  bwputr(TERMINAL, tf->r4);
  bwputr(TERMINAL, tf->r5);
  bwputr(TERMINAL, tf->r6);
  bwputr(TERMINAL, tf->r7);
  putc(TERMINAL, '\n');
  putc(TERMINAL, '\r');
  bwputr(TERMINAL, tf->r8);
  bwputr(TERMINAL, tf->r9);
  bwputr(TERMINAL, tf->r10);
  bwputr(TERMINAL, tf->fp);
  putc(TERMINAL, '\n');
  putc(TERMINAL, '\r');
  bwputr(TERMINAL, tf->ip);
  bwputr(TERMINAL, tf->sp);
  bwputr(TERMINAL, tf->lr);
  bwputr(TERMINAL, tf->pc);
  bwputr(TERMINAL, tf->k_lr);
  bwputr(TERMINAL, tf->psr);
  putc(TERMINAL, '\n');
  putc(TERMINAL, '\r');
}

unsigned int tasks_ended;

trapframe *handle_interrupt(trapframe *tf) {
  switch (tf->r0) {
    case SYS_EXIT:
      syscall_exit();
      // tf->r0 = 0;
      // DON'T DO THE ABOVE HERE YOU ASSHOLE. DO IT FOR OTHER SYSCALLS BUT NOT THIS ONE.
      break;
    default:
      tf->r0 = 0xCAFED00D;
  }
#if TRAPFRAME_DEBUG
  bwprintf("End of handle_interrupt\n\r");
  print_tf(tf);
#endif /* TRAPFRAME_DEBUG */
  return tf;
}
