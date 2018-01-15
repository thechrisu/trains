#if VERSATILEPB
#include "cp_vec.h"
#endif

#include "crash.h"
#include "tasks.h"
#include "interrupt.h"
#include "stdlib.h"
#include "myio.h"

#ifndef VERSATILEPB
extern void enter_kernel(unsigned int syscall_code);
#endif

extern unsigned int *stack_pointers;
extern unsigned int current_task;
extern unsigned int tasks_ended;
extern void sys_exit();
extern void leave_main(int ret_code, trapframe *tf);

int main() {
  setup_io();
  current_task = 0;
  tasks_ended = 0;
  unsigned int local_sps[2];
  stack_pointers = local_sps;

#ifndef VERSATILEPB
  uint32_t *swi_handler = (uint32_t *)0x28;
  *swi_handler = (uint32_t)(&enter_kernel);
#endif /* VERSATILEPB */

#if VERSATILEPB
  uint32_t *first_user_task_stack = (uint32_t *)(0x01FFFFFF);
#else
  uint32_t *first_user_task_stack = (uint32_t *)(0x00fdd000);
#endif /* VERSTILEPB */

  trapframe *tf = (trapframe *)((uint32_t)first_user_task_stack - sizeof(trapframe));
  tf->r1 = 0x1EADBEE0;
  tf->r2 = 0x1EADBEE1;
  tf->r3 = 0x1EADBEE2;
  tf->r4 = 0x1EADBEE3;
  bwprintf("Setting tf, mem: %x. TF at %x\n\r", &(tf->r5), tf);
  tf->r5 = 0x1EADBEE4;
  tf->r6 = 0x1EADBEE5;
  tf->r7 = 0x1EADBEE6;
  tf->r8 = 0x1EADBEE7;
  tf->r9 = 0x1EADBEE8;
  tf->r10 = 0x1EADBEE9;
  tf->fp = 0x1EADBEEA;
  tf->ip = 0x1EADBEEB;
  tf->sp = (uint32_t)tf;
  tf->lr = (uint32_t)(&sys_exit);
  tf->pc = (uint32_t)(&first_user_task);
  tf->k_lr = (uint32_t)(&first_user_task);

#ifdef CONTEXT_SWITCH_DEBUG
  bwprintf("MAIN");

  bwputr(TERMINAL, (uint32_t)tf);
  putc(TERMINAL, '\n');
  putc(TERMINAL, '\r');
#endif /* CONTEXT_SWITCH_DEBUG */

#if VERSATILEPB
  uint32_t *second_user_task_stack = (uint32_t *)(0x01FCFFFF);
#else
  uint32_t *second_user_task_stack = (uint32_t *)(0x00fad000);
#endif /* VERSATILEPB */

  trapframe *tf2 = (trapframe *)((uint32_t)second_user_task_stack - sizeof(trapframe));
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
  tf2->lr = (uint32_t)(&sys_exit);
  tf2->pc = (uint32_t)(&second_user_task);
  tf2->k_lr = (uint32_t)(&second_user_task);

  stack_pointers[0] = (uint32_t)tf;
  stack_pointers[1] = (uint32_t)tf2;

#ifdef CONTEXT_SWITCH_DEBUG
  print_tf(tf);
  print_tf(tf2);
  bwprintf("Stackpointers: %x, Current: %x\n\r", stack_pointers, &current_task);
  bwprintf("IN MAIN: (%x, %x) with sps: (%x, %x)\n\r", (uint32_t)tf, (uint32_t)tf2, stack_pointers[0], stack_pointers[1]);
#endif /* CONTEXT_SWITCH_DEBUG */

  leave_main(0, tf);

  bwprintf("Return from get_me_outta_here\n\r");
#if VERSATILEPB
  CRASH();
#else
  return 0;
#endif /* VERSATILEPB */
}
