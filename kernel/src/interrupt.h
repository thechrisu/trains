/**
 * @file
 * @brief Interrupt handler
 */
#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "codes.h"
#include "tstdlib.h"
#include "kusage_stats.h"
#include "usage_stats.h"
#include "iio.h"
#include "vic.h"

#if CONTEXT_SWITCH_BENCHMARK
#include "benchmark.h"
#endif /* CONTEXT_SWITCH_BENCHMARK */

typedef struct {
  register_t r0;
  register_t r1;
  register_t r2;
  register_t r3;
  register_t r4;
  register_t r5;
  register_t r6;
  register_t r7;
  register_t r8;
  register_t r9;
  register_t r10;
  register_t fp;
  register_t ip;
  register_t sp;
  register_t lr;
  register_t pc;
  register_t k_lr;
  register_t psr;
} trapframe;

extern int num_syscalls[NUM_SYSCALL_CODES];
extern int num_syscalls_total;

#define NULL_TRAPFRAME (trapframe *)0

/**
 * Utility function to print a trapframe.
 *
 * @param tf The trapframe to print.
 */
void print_tf(trapframe *tf);
#endif /* INTERRUPT_H */
