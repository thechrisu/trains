#include "parameters.h"

#define NUM_ACTIVE_TRAINS_DEFAULT 4
#define SWITCH_PADDING_DEFAULT    50
#define SPACING_DEFAULT           100
#define SPACING_ERROR_DEFAULT     10

int active_trains[81];
int num_active_trains;

unsigned int expected_next_sensors[81];

int switch_padding;

int spacing;

int spacing_error;

static char parameter_names[MAX_PARAMETER][PARAMETER_NAME_LENGTH];

void init_parameters() {
  active_trains[0] = 24;
  active_trains[1] = 58;
  active_trains[2] = 74;
  active_trains[3] = 78;

  for (int i = 0; i < 81; i += 1) {
    expected_next_sensors[i] = NO_NEXT_SENSOR_EXPECTED;
  }
  expected_next_sensors[24] = sensor_offset('A', 5);
  expected_next_sensors[58] = sensor_offset('A', 8);
  expected_next_sensors[74] = sensor_offset('A', 8);
  expected_next_sensors[78] = sensor_offset('A', 8);

  num_active_trains = NUM_ACTIVE_TRAINS_DEFAULT;
  switch_padding = SWITCH_PADDING_DEFAULT;
  spacing = SPACING_DEFAULT;
  spacing_error = SPACING_ERROR_DEFAULT;

  tmemcpy(parameter_names[SET_TRAINS], "trains", sizeof("trains"));
  tmemcpy(parameter_names[SET_SWITCH_PADDING], "switch_padding", sizeof("switch_padding"));
  tmemcpy(parameter_names[SET_TRACK], "track", sizeof("track"));
  tmemcpy(parameter_names[SET_DIRECTION], "direction", sizeof("direction"));
  tmemcpy(parameter_names[SET_SPACING], "spacing", sizeof("spacing"));
  tmemcpy(parameter_names[SET_SPACING_ERROR], "spacing_error",
          sizeof("spacing_error"));
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
