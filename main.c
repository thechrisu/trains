#include "cp_vec.h"
#include "crash.h"
#include "tasks.h"
#include "interrupt.h"
#include "stdlib.h"
#include "myio.h"


extern unsigned int *stack_pointers;
extern unsigned int current_task;

int main() {
  setup_io();
  current_task = 0;
  unsigned int local_sps[2];
  stack_pointers = local_sps;

#ifndef VERSATILEPB
  uint32_t *swi_handler = (uint32_t *)0x28;
  *swi_handler = (uint32_t)(&enter_kernel);
#endif

  uint32_t *first_user_task_stack = (uint32_t *)(0x01FFFFFF);

  struct trapframe *tf = (struct trapframe *)((uint32_t)first_user_task_stack - sizeof(struct trapframe));
  tf->r1 = 0x2EADBEE0;
  tf->r2 = 0x2EADBEE1;
  tf->r3 = 0x2EADBEE2;
  tf->r4 = 0x2EADBEE3;
  tf->r5 = 0x1EADBEE4;
  tf->r6 = 0x1EADBEE5;
  tf->r7 = 0x1EADBEE6;
  tf->r8 = 0x1EADBEE7;
  tf->r9 = 0x1EADBEE8;
  tf->r10 = 0x1EADBEE9;
  tf->fp = 0x1EADBEEA;
  tf->ip = 0x1EADBEEB;
  tf->sp = (uint32_t)tf;
  tf->lr = 0x1EADBEED;
  tf->pc = (uint32_t)(&first_user_task);
  tf->k_lr = (uint32_t)(&first_user_task);

  int x = 1;
  bwprintf("MAIN");

  bwputr(TERMINAL, (uint32_t)tf);
  putc(TERMINAL, '\n');
  uint32_t *second_user_task_stack = (uint32_t *)(0x01FCFFFF);

  struct trapframe *tf2 = (struct trapframe *)((uint32_t)second_user_task_stack - sizeof(struct trapframe));
  tf2->r1 = 0x2EADBEE0;
  tf2->r2 = 0x2EADBEE1;
  tf2->r3 = 0x2EADBEE2;
  tf2->r4 = 0x2EADBEE3;
  tf2->r5 = 0x2EADBEE4;
  tf2->r6 = 0x2EADBEE5;
  tf2->r7 = 0x2EADBEE6;
  tf2->r8 = 0x2EADBEE7;
  tf2->r9 = 0x2EADBEE8;
  tf2->r10 = 0x2EADBEE9;
  tf2->fp = 0x2EADBEEA;
  tf2->ip = 0x2EADBEEB;
  tf2->sp = (uint32_t)tf2;
  tf2->lr = 0x2EADBEED;
  tf2->pc = (uint32_t)(&second_user_task);
  tf2->k_lr = (uint32_t)(&second_user_task);

  stack_pointers[0] = (uint32_t)tf;
  stack_pointers[1] = (uint32_t)tf2;

  bwprintf("Stackpointers: %x, Current: %x\n", stack_pointers, &current_task);
  bwprintf("IN MAIN: (%x, %x) with sps: (%x, %x)\n", (uint32_t)tf, (uint32_t)tf2, stack_pointers[0], stack_pointers[1]);

  leave_kernel(0, tf);
  CRASH();
  /* Should never reach */
  return 0;
}
