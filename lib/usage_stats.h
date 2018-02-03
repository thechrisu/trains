#ifndef LUSAGE_STATS_H
#define LUSAGE_STATS_H

#include "constants.h"
#include "tstdlib.h"

typedef struct {
  int32_t ms_run[MAX_TASKS + 1];
  int max_tid;
} usage_stats;

#endif /* LUSAGE_STATS_H */
