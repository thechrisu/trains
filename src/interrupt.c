#include "crash.h"
#include "interrupt.h"
#include "myio.h"
#include "tasks.h"

unsigned int *kernel_stack_pointer;

unsigned int *stack_pointers;
unsigned int current_task;

void print_tf(struct trapframe *tf) {
  //bwprintf("TF AT LOC %x\n", tf);
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

void handle_interrupt(struct trapframe *tf) {
  //bwprintf("current loc %x\n", &current_task);
  current_task = current_task % 2;
  stack_pointers[current_task] = (uint32_t)tf;
  //bwputr(TERMINAL, *((uint32_t *)0x0));
  //putc(TERMINAL, '\n');
  bwprintf("current: %d\n", current_task);
  print_tf(tf);
  current_task = (current_task + 1) % 2;
  print_tf(stack_pointers[current_task]);
  bwprintf("current: %d\n", current_task);

  /*__asm__(
    "MOV r0, #0\n\t"
    "ldr r1, r3\n\t"
  :
  :
  );
  goto leave_kernel;*/
  leave_kernel(0, stack_pointers[current_task]);
  // CRASH();

   /*bwputr(TERMINAL, (uint32_t)(&second_user_task));
   bwputr(TERMINAL, (uint32_t)tf);
   putc(TERMINAL, '0' + ((uint32_t)tf == 0x01FFFFAF));
   putc(TERMINAL, '\n');*/

  /*if ((uint32_t)tf == 0x01FFFFBF) {
    bwprintf("INTERRUPT TO SECOND");
    leave_kernel(0, 0x01FCFFBF);
  } else {
    bwprintf("INTERRUPT TO FIRST");
    leave_kernel(0, 0x01FFFFBF);
  }*/
}
