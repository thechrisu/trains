#include "parameters.h"

#define NUM_ACTIVE_TRAINS_DEFAULT 3
#define SWITCH_PADDING_DEFAULT    50

int active_trains[81];
int num_active_trains;

int switch_padding;

static char parameter_names[MAX_PARAMETER][PARAMETER_NAME_LENGTH];

void init_parameters() {
  active_trains[0] = 24;
  active_trains[1] = 58;
  active_trains[2] = 74;

  num_active_trains = NUM_ACTIVE_TRAINS_DEFAULT;
  switch_padding = SWITCH_PADDING_DEFAULT;

  tmemcpy(parameter_names[SET_TRAINS], "trains", sizeof("trains"));
  tmemcpy(parameter_names[SET_SWITCH_PADDING], "switch_padding", sizeof("switch_padding"));
}

char *get_parameter_name(set_parameter index) {
  return parameter_names[index];
}
