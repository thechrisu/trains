#include "parameters.h"

#define NUM_ACTIVE_TRAINS_DEFAULT 3
#define SWITCH_PADDING_DEFAULT    50

int active_trains[81];
int num_active_trains;

unsigned int expected_next_sensors[81];

int switch_padding;

static char parameter_names[MAX_PARAMETER][PARAMETER_NAME_LENGTH];

void init_parameters() {
  active_trains[0] = 24;
  active_trains[1] = 58;
  active_trains[2] = 74;

  for (int i = 0; i < 81; i += 1) {
    expected_next_sensors[i] = NO_NEXT_SENSOR_EXPECTED;
  }
  expected_next_sensors[24] = sensor_offset('A', 5);
  expected_next_sensors[74] = sensor_offset('A', 8);

  num_active_trains = NUM_ACTIVE_TRAINS_DEFAULT;
  switch_padding = SWITCH_PADDING_DEFAULT;

  tmemcpy(parameter_names[SET_TRAINS], "trains", sizeof("trains"));
  tmemcpy(parameter_names[SET_SWITCH_PADDING], "switch_padding", sizeof("switch_padding"));
  tmemcpy(parameter_names[SET_TRACK], "track", sizeof("track"));
  tmemcpy(parameter_names[SET_DIRECTION], "direction", sizeof("direction"));
}

char *get_parameter_name(set_parameter index) {
  return parameter_names[index];
}

set_parameter get_parameter_value(char *name) {
  for (set_parameter i = MIN_PARAMETER; i < MAX_PARAMETER; i += 1) {
    if (tstrcmp(name, get_parameter_name(i))) {
      return i;
    }
  }

  return MAX_PARAMETER;
}
