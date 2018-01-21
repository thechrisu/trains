#include "../include/common/codes.h"
#include "./syscall/syscall.h"
#include "./multitasking/messaging.h"
#include "crash.h"
#include "interrupt.h"
#include "myio.h"

#if TESTING
extern void syscall_exit();
extern void syscall_pass();
extern int syscall_mytid();
extern int syscall_myparenttid();
#endif /* TESTING */

void print_tf(trapframe *tf) {
#ifndef TESTING
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
#endif /* TESTING */
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
#ifndef TESTING
      tf->r0 = syscall_create(tf->r1, (void(*)(void))tf->r2);
#endif /* TESTING */
      break;
    case SYS_MYTID:
      tf->r0 = syscall_mytid();
      break;
    case SYS_PARENTTID:
      tf->r0 = syscall_myparent_tid();
      break;
    case SYS_PANIC:
      syscall_panic();
      break;
    case SYS_SEND:
      syscall_send();
      break;
    case SYS_RECEIVE:
      syscall_receive();
      break;
    case SYS_REPLY:
      syscall_reply();
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
