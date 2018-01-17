#include "../include/common/codes.h"
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

trapframe *handle_interrupt(trapframe *tf) {
  switch (tf->r0) {
    case SYS_EXIT:
      syscall_exit();
      break;
    case SYS_PASS:
      syscall_pass();
      break;
    case SYS_CREATE:
      tf->r0 = syscall_create(tf->r1, (void(*)(void))tf->r2);
      break;
    case SYS_MYTID:
      tf->r0 = syscall_mytid();
      break;
    case SYS_PARENTTID:
      tf->r0 = syscall_myparent_tid();
      break;
    case SYS_ABORT:
      syscall_abort();
      break;
    default:
      tf->r0 = 0xABADC0DE;
  }
#if TRAPFRAME_DEBUG
  bwprintf("End of handle_interrupt\n\r");
  print_tf(tf);
#endif /* TRAPFRAME_DEBUG */
  return tf;
}
