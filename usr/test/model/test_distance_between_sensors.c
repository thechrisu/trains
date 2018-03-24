#include "test_model.h"

void test_distance_between_sensors() {
  track_state track;
  init_track(&track);

  Assert(distance_between_sensors(find_sensor(&track, sensor_offset('A', 1)), find_sensor(&track, sensor_offset('A', 1))) == 0);
  Assert(distance_between_sensors(find_sensor(&track, sensor_offset('A', 5)), find_sensor(&track, sensor_offset('C', 7))) == 359);
  Assert(distance_between_sensors(find_sensor(&track, sensor_offset('E', 13)), find_sensor(&track, sensor_offset('D', 15))) == 289);
  Assert(distance_between_sensors(find_sensor(&track, sensor_offset('C', 8)), find_sensor(&track, sensor_offset('A', 7))) == 542);
  Assert(distance_between_sensors(find_sensor(&track, sensor_offset('B', 16)), find_sensor(&track, sensor_offset('C', 5))) == 483);
  Assert(distance_between_sensors(find_sensor(&track, sensor_offset('E', 1)), find_sensor(&track, sensor_offset('C', 1))) == 485);
  Assert(distance_between_sensors(find_sensor(&track, sensor_offset('C', 7)), find_sensor(&track, sensor_offset('E', 11))) == 807);

  bwprintf("Success.\n\r");
}
