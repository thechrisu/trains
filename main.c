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
extern trapframe *main_trapframe;
extern void sys_exit();
extern void leave_main(int ret_code, trapframe *tf);

int main() {
  setup_io();

#ifndef VERSATILEPB
  uint32_t *swi_handler = (uint32_t *)0x28;
  *swi_handler = (uint32_t)(&enter_kernel);
#endif /* VERSATILEPB */

  // TODO call syscall_create to create first user task

  while(schedule());

#if VERSATILEPB
  CRASH();
#else
  return 0;
#endif /* VERSATILEPB */
}
