#include "crash.h"
#include "interrupt.h"
#include "myio.h"
#include "stdlib.h"
#include "tasks.h"

#define kassert(expr) __kassert((bool)(expr), __FUNCTION__, __FILE__, __LINE__)

unsigned int handle_interrupt_fp;
unsigned int handle_interrupt_sp;
bool assertion_failed;

void __kassert(bool value, const char * caller_name, const char *file_name, int line_num) {
  if (!value) {
    bwprintf("Assertion failed! \"%s\" at %s:%d\n", caller_name, file_name, line_num);
    assertion_failed = true;
    __asm__(
      "mov fp, %0\n\t"
      "mov sp, %1\n\t"
      "b kassert_exit"
    : : "r" (handle_interrupt_fp), "r" (handle_interrupt_sp));
  }
}

extern void leave_kernel(int ret_code, trapframe *tf);
extern void get_me_outta_here();

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
    kassert(false);
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
