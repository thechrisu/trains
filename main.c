#include "cp_vec.h"
#include "crash.h"
#include "first_user_task.h"
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

  leave_kernel(0, tf);

  /* Should never reach */
  return 0;
}
