#include "test_model.h"

void test_sensors_are_paired() {
  track_state track;
  init_track(&track);

  Assert(!sensors_are_paired(&track, sensor_offset('A', 1), sensor_offset('A', 1)));
  Assert(!sensors_are_paired(&track, sensor_offset('B', 11), sensor_offset('A', 8)));
  Assert(sensors_are_paired(&track, sensor_offset('A', 1), sensor_offset('A', 2)));

  bwprintf("Success.\n\r");
}
