/**
 * @file
 * @brief Module responsible for calculating runtime breakdowns.
 */
#ifndef KUSAGE_STATS_H
#define KUSAGE_STATS_H

#include "int32_t_buffer.h"
#include "mytimer.h"
#include "task.h"
#include "tstdlib.h"
#include "usage_stats.h"

#define KERNEL_FAKE_TID 0

/**
 * Sets up the buffers tracking which tasks ran in the past second.
 */
void setup_kusage_stats();

/**
 * If more than 1 second has passed since last tallying up last seconds' performance,
 * Start over tallying up last seconds' performance.
 */
void maybe_empty_last_secs_buffer();

/**
 * Indicate that a new task runs now.
 */
void start_interval();

/**
 * Indicate that task with tid tid has finished running.
 * ALWAYS call <code>end_interval()</code> after <code>start_interval()</code>.
 */
void end_interval(int32_t tid);

/**
 * Kernel-level implementation of <code>TotalProcUsage()</code> syscall.
 */
void syscall_total_proc_usage(usage_stats *t_usage);

/**
 * Kernel-level implementation of <code>LastSecsProcUsage()</code> syscall.
 */
void syscall_last_secs_proc_usage(usage_stats *t_usage);

#endif /* KUSAGE_STATS_H */
