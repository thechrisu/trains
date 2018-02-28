#include "parameters.h"

#define T1TRAIN_DEFAULT 74

int t1train;

static char parameter_names[MAX_PARAMETER][10];

void init_parameters() {
  t1train = T1TRAIN_DEFAULT;

  tmemcpy(parameter_names[SET_T1TRAIN], "t1train", sizeof("t1train"));
}

char *get_parameter_name(set_parameter index) {
  return parameter_names[index];
}
