#include "crash.h"
#include "interrupt.h"
#include "myio.h"
#include "tasks.h"

extern void leave_kernel(int ret_code, trapframe *tf);
extern void get_me_outta_here();

unsigned int *stack_pointers;
unsigned int current_task;

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
  putc(TERMINAL, '\n');
  putc(TERMINAL, '\r');
}

unsigned int tasks_ended;

void handle_interrupt(trapframe *tf) {
  current_task = current_task % 2;
  stack_pointers[current_task] = (uint32_t)tf;
#ifdef CONTEXT_SWITCH_DEBUG
  bwprintf("Syscall argument: %x\n\r", tf->r0);
  bwprintf("current: %d\n\r", current_task);
  print_tf((trapframe *)tf);
#endif /* CONTEXT_SWITCH_DEBUG */
  current_task = (current_task + 1) % 2;
#ifdef CONTEXT_SWITCH_DEBUG
  bwprintf("current: %d\n\r", current_task);
  print_tf((trapframe *)stack_pointers[current_task]);
#endif /* CONTEXT_SWITCH_DEBUG */
  if (tf->r0 == 0) {
    bwprintf("Exit code: %x\n\r", tf->r1);
    tasks_ended += 1;
  }
  if (tasks_ended == 2) {
    __asm__("add sp, sp, #32");
    return;
  }

  volatile int k_sp;
  bwprintf("Something in the current stack frame: %x\n\r", &k_sp);
  __asm__("add sp, sp, #32");

  leave_kernel(current_task + 1, (trapframe *)stack_pointers[current_task]);
}
