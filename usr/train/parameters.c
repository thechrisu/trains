#include "parameters.h"

#define T1TRAIN_DEFAULT 74
#define SWITCH_PADDING_DEFAULT 50

int t1train;
int switch_padding;

static char parameter_names[MAX_PARAMETER][PARAMETER_NAME_LENGTH];

void init_parameters() {
  t1train = T1TRAIN_DEFAULT;
  switch_padding = SWITCH_PADDING_DEFAULT;

  tmemcpy(parameter_names[SET_T1TRAIN], "t1train", sizeof("t1train"));
  tmemcpy(parameter_names[SET_SWITCH_PADDING], "switch_padding", sizeof("switch_padding"));
}

char *get_parameter_name(set_parameter index) {
  return parameter_names[index];
}
