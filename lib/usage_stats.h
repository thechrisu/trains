#ifndef LUSAGE_STATS_H
#define LUSAGE_STATS_H

#include "constants.h"
#include "tstdlib.h"

typedef struct {
  uint32_t ms_run[MAX_TASKS + 1];
  int max_tid;
} usage_stats;

void print_usage(void (*print)(char *,...), usage_stats* usage);

#endif /* LUSAGE_STATS_H */
