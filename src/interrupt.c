#include "assert.h"
#include "crash.h"
#include "interrupt.h"
#include "myio.h"
#include "stdlib.h"
#include "tasks.h"

unsigned int handle_interrupt_fp;
unsigned int handle_interrupt_sp;
bool assertion_failed;

trapframe *main_trapframe;
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
  __asm__(
    "mov %0, fp\n\t"
    "mov %1, sp\n\t"
  : "=r" (handle_interrupt_fp), "=r" (handle_interrupt_sp));

  assertion_failed = false;

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
#ifdef CONTEXT_SWITCH_DEBUG
    bwprintf("Exit code: %x\n\r", tf->r1);
#endif /* CONTEXT_SWITCH_DEBUG */
    tasks_ended += 1;
  }
  trapframe *next = (trapframe *)stack_pointers[current_task];
  if (tasks_ended == 2) {
#ifdef CONTEXT_SWITCH_DEBUG
    bwprintf("Both tasks finished!\n\r");
#endif /* CONTEXT_SWITCH_DEBUG */
    next = main_trapframe;
  }
#ifdef CONTEXT_SWITCH_DEBUG
  volatile int k_sp;
  bwprintf("Something in the current stack frame: %x\n\r", &k_sp);
#endif /* CONTEXT_SWITCH_DEBUG */

  __asm__(
    ".text\n\t"
    ".global kassert_exit\n\t"
    "kassert_exit:\n\t"
  );

  if (assertion_failed)
    next = main_trapframe;

  __asm__ volatile(
  "MOV r0, %0\n\t"
  "MOV r1, %1\n\t"
  "MOV r2, %2\n\t"
  : :"r"(current_task + 1), "r"(next), "r"(next == main_trapframe) : "r0", "r1", "r2");
}
