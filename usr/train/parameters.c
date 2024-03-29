#include "parameters.h"

#define NUM_ACTIVE_TRAINS_DEFAULT         6
#define SWITCH_PADDING_DEFAULT            50
#define SPACING_DEFAULT                   100
#define SPACING_ERROR_DEFAULT             10
#define SPACING_CATCHUP_TIME_DEFAULT      100 // Ticks

int active_trains[81];
int num_active_trains;

unsigned int expected_next_sensors[81];

int switch_padding;

int spacing;

int spacing_error;

int spacing_catchup_time;

static char parameter_names[MAX_PARAMETER][PARAMETER_NAME_LENGTH];

void init_parameters() {
  active_trains[0] = 24;
  active_trains[1] = 58;
  active_trains[2] = 74;
  active_trains[3] = 77;
  active_trains[4] = 78;
  active_trains[5] = 79;

  for (int i = 0; i < 81; i += 1) {
    expected_next_sensors[i] = NO_NEXT_SENSOR_EXPECTED;
  }

  num_active_trains = NUM_ACTIVE_TRAINS_DEFAULT;
  switch_padding = SWITCH_PADDING_DEFAULT;
  spacing = SPACING_DEFAULT;
  spacing_error = SPACING_ERROR_DEFAULT;
  spacing_catchup_time = SPACING_CATCHUP_TIME_DEFAULT;

  tmemcpy(parameter_names[SET_TRAINS], "trains", sizeof("trains"));
  tmemcpy(parameter_names[SET_SWITCH_PADDING], "switch_padding", sizeof("switch_padding"));
  tmemcpy(parameter_names[SET_TRACK], "track", sizeof("track"));
  tmemcpy(parameter_names[SET_DIRECTION], "direction", sizeof("direction"));
  tmemcpy(parameter_names[SET_SPACING], "spacing", sizeof("spacing"));
  tmemcpy(parameter_names[SET_SPACING_ERROR], "spacing_error", sizeof("spacing_error"));
  tmemcpy(parameter_names[SET_SPACING_CATCHUP_TIME], "spacing_catchup_time",
          sizeof("spacing_catchup_time"));
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
