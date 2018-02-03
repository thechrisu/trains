#ifndef KUSAGE_STATS_H
#define KUSAGE_STATS_H

#include "int32_t_buffer.h"
#include "mytimer.h"
#include "task.h"
#include "tstdlib.h"
#include "usage_stats.h"

#define KERNEL_FAKE_TID 0

void setup_kusage_stats();
void maybe_empty_last_secs_buffer();
void start_interval();
void end_interval(int32_t tid);
void syscall_total_proc_usage(usage_stats *t_usage);
void syscall_last_secs_proc_usage(usage_stats *t_usage);

#endif /* KUSAGE_STATS_H */
