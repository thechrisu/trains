/**
 * @file
 * @brief Interrupt handler
 */
#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "stdlib.h"

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
} trapframe;

#define NULL_TRAPFRAME (trapframe *)0

void print_tf(trapframe *tf);
#endif /* INTERRUPT_H */
