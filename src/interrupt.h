/**
 * @file
 * @brief Interrupt handler
 */
#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "stdlib.h"
#if TESTING
  #include "multitasking/task.h"
  typedef struct {
    uint64_t r0;
    uint64_t r1;
    uint64_t r2;
    uint64_t r3;
    uint64_t r4;
    uint64_t r5;
    uint64_t r6;
    uint64_t r7;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t fp;
    uint64_t ip;
    uint64_t sp;
    uint64_t lr;
    uint64_t pc;
    uint64_t k_lr;
    uint64_t psr;
  } trapframe;
#else
  typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t fp;
    uint32_t ip;
    uint32_t sp;
    uint32_t lr;
    uint32_t pc;
    uint32_t k_lr;
    uint32_t psr;
  } trapframe;
#endif /* TESTING */


#define NULL_TRAPFRAME (trapframe *)0

void print_tf(trapframe *tf);
#endif /* INTERRUPT_H */
