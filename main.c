#include "cp_vec.h"
#include "crash.h"
#include "tasks.h"
#include "interrupt.h"
#include "stdlib.h"
#include "myio.h"

int main() {
  setup_io();

#ifndef VERSATILEPB
  uint32_t *swi_handler = (uint32_t *)0x28;
  *swi_handler = (uint32_t)(&enter_kernel);
#endif

  uint32_t *first_user_task_stack = (uint32_t *)(0x01FFFFFF);

  trapframe *tf = (trapframe *)((uint32_t)first_user_task_stack - sizeof(trapframe));
  tf->r1 = 0;
  tf->r2 = 0;
  tf->r3 = 0;
  tf->r4 = 0;
  tf->r5 = 0;
  tf->r6 = 0;
  tf->r7 = 0;
  tf->r8 = 0;
  tf->r9 = 0;
  tf->r10 = 0;
  tf->fp = 0;
  tf->ip = 0;
  tf->sp = (uint32_t)tf;
  tf->lr = (uint32_t)(&first_user_task);
  tf->pc = 0;

  uint32_t *second_user_task_stack = (uint32_t *)(0x01FCFFFF);

  trapframe *tf2 = (trapframe *)((uint32_t)second_user_task_stack - sizeof(trapframe));
  tf2->r1 = 0xDEADBEEF;
  tf2->r2 = 0;
  tf2->r3 = 0;
  tf2->r4 = 0;
  tf2->r5 = 0;
  tf2->r6 = 0;
  tf2->r7 = 0;
  tf2->r8 = 0;
  tf2->r9 = 0;
  tf2->r10 = 0;
  tf2->fp = 0;
  tf2->ip = 0;
  tf2->sp = (uint32_t)tf2;
  tf2->lr = (uint32_t)(&second_user_task);
  tf2->pc = 0;

  leave_kernel(0, tf);

  /* Should never reach */
  return 0;
}
