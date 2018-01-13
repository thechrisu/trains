#include "crash.h"
#include "interrupt.h"
#include "myio.h"
#include "tasks.h"

unsigned int *kernel_stack_pointer;

void handle_interrupt(trapframe *tf) {
  // bwputr(TERMINAL, tf->r0);
  // bwputr(TERMINAL, tf->r1);
  // bwputr(TERMINAL, tf->r2);
  // bwputr(TERMINAL, tf->r3);
  // bwputr(TERMINAL, tf->r4);
  // bwputr(TERMINAL, tf->r5);
  // bwputr(TERMINAL, tf->r6);
  // bwputr(TERMINAL, tf->r7);
  // bwputr(TERMINAL, tf->r8);
  // bwputr(TERMINAL, tf->r9);
  // bwputr(TERMINAL, tf->r10);
  // bwputr(TERMINAL, tf->fp);
  // bwputr(TERMINAL, tf->ip);
  // bwputr(TERMINAL, tf->sp);
  // bwputr(TERMINAL, tf->lr);
  // bwputr(TERMINAL, tf->pc);

  // bwputr(TERMINAL, (uint32_t)(&second_user_task));
  // bwputr(TERMINAL, (uint32_t)tf);
  // putc(TERMINAL, '0' + ((uint32_t)tf == 0x01FFFFAF));
  // putc(TERMINAL, '\n');

  if ((uint32_t)tf == 0x01FFFFCF)
    leave_kernel(0, 0x01FCFFCF);
  else
    leave_kernel(0, 0x01FFFFCF);
}
