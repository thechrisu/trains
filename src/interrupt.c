#include "crash.h"
#include "interrupt.h"
#include "myio.h"
#include "tasks.h"

extern void leave_kernel(int ret_code, trapframe *tf);

unsigned int *stack_pointers;
unsigned int current_task;

void print_tf(trapframe *tf) {
  bwputr(TERMINAL, tf->r0);
  bwputr(TERMINAL, tf->r1);
  bwputr(TERMINAL, tf->r2);
  bwputr(TERMINAL, tf->r3);
  putc(TERMINAL, '\n');
  bwputr(TERMINAL, tf->r4);
  bwputr(TERMINAL, tf->r5);
  bwputr(TERMINAL, tf->r6);
  bwputr(TERMINAL, tf->r7);
  putc(TERMINAL, '\n');
  bwputr(TERMINAL, tf->r8);
  bwputr(TERMINAL, tf->r9);
  bwputr(TERMINAL, tf->r10);
  bwputr(TERMINAL, tf->fp);
  putc(TERMINAL, '\n');
  bwputr(TERMINAL, tf->ip);
  bwputr(TERMINAL, tf->sp);
  bwputr(TERMINAL, tf->lr);
  bwputr(TERMINAL, tf->pc);
  bwputr(TERMINAL, tf->k_lr);
  putc(TERMINAL, '\n');
}

void handle_interrupt(trapframe *tf) {
  current_task = current_task % 2;
  stack_pointers[current_task] = (uint32_t)tf;
#ifdef CONTEXT_SWITCH_DEBUG
  bwprintf("r0: %x\n", tf->r0);
  bwprintf("current: %d\n", current_task);
  print_tf((trapframe *)tf);
#endif /* CONTEXT_SWITCH_DEBUG */
  current_task = (current_task + 1) % 2;
#ifdef CONTEXT_SWITCH_DEBUG
  bwprintf("current: %d\n", current_task);
  print_tf((trapframe *)stack_pointers[current_task]);
#endif /* CONTEXT_SWITCH_DEBUG */

  leave_kernel(current_task + 1, (trapframe *)stack_pointers[current_task]);
}
