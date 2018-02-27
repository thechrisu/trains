#include "test_model.h"

void test_distance_between_sensors() {
  track_state track;
  init_track(&track);

  Assert(distance_between_sensors(&track, sensor_offset('A', 1), sensor_offset('A', 1)) == 0);
  Assert(distance_between_sensors(&track, sensor_offset('A', 5), sensor_offset('C', 7)) == 359);
  Assert(distance_between_sensors(&track, sensor_offset('E', 13), sensor_offset('D', 15)) == 289);
  Assert(distance_between_sensors(&track, sensor_offset('C', 8), sensor_offset('A', 7)) == 542);
  Assert(distance_between_sensors(&track, sensor_offset('B', 16), sensor_offset('C', 5)) == 483);
  Assert(distance_between_sensors(&track, sensor_offset('E', 1), sensor_offset('C', 1)) == 485);
  Assert(distance_between_sensors(&track, sensor_offset('C', 7), sensor_offset('E', 11)) == 807);

  bwprintf("Success.\n\r");
}
