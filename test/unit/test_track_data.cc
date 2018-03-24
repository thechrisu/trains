#include "test_track_data.h"
#include <gtest/gtest.h>

using namespace std;

#ifndef ALLTESTS
int oe_in_sensor = false;
#endif /* ALLTESTS */

int NUM_TESTS = 10;
char banks[]   = { 'A',  'B',  'C', 'D', 'E',  'A',  'B',  'C',  'D', 'E'  };
int indices[]  = {  1,    3,    5,   8,   2,    9,    12,   10,   15,  16  };
int offsets[]  = {  0,    18,   36,  55,  65,   8,    27,   41,   62,  79  };
int elements[] = {  0,    2,    4,   6,   8,    1,    3,    5,    7,   9   };
int masks[]    = {  0x80, 0x20, 0x8, 0x1, 0x40, 0x80, 0x10, 0x40, 0x2, 0x1 };

void init_turnouts(turnout_state turnouts[NUM_TURNOUTS]) {
  for (unsigned int i = 0; i < NUM_TURNOUTS; i += 1) {
    unsigned int turnout_num = map_offset_to_turnout(i);
    turnouts[i] = (turnout_num == 153 || turnout_num == 155) ? TURNOUT_STRAIGHT : TURNOUT_CURVED;
  }
}

TEST(TrackDataTest, test_sensor_bank) {
  for (int i = 0; i < NUM_TESTS; i += 1) {
    ASSERT_EQ(banks[i], sensor_bank(offsets[i]));
  }
}

TEST(TrackDataTest, test_sensor_index) {
  for (int i = 0; i < NUM_TESTS; i += 1) {
    ASSERT_EQ(indices[i], sensor_index(offsets[i]));
  }
}

TEST(TrackDataTest, test_sensor_offset) {
  for (int i = 0; i < NUM_TESTS; i += 1) {
    ASSERT_EQ(offsets[i], sensor_offset(banks[i], indices[i]));
  }
}

TEST(TrackDataTest, test_sensor_data_element) {
  for (int i = 0; i < NUM_TESTS; i += 1) {
    ASSERT_EQ(elements[i], sensor_data_element(offsets[i]));
  }
}

TEST(TrackDataTest, test_sensor_data_mask) {
  for (int i = 0; i < NUM_TESTS; i += 1) {
    ASSERT_EQ(masks[i], sensor_data_mask(offsets[i]));
  }
}

TEST(TrackDataTest, test_bank_and_index_to_offset_and_back) {
  for (char bank = 'A'; bank <= 'E'; bank += 1) {
    for (int index = 1; index <= 16; index += 1) {
      ASSERT_EQ(bank, sensor_bank(sensor_offset(bank, index)));
      ASSERT_EQ(index, sensor_index(sensor_offset(bank, index)));
    }
  }
}

TEST(TrackDataTest, test_offset_to_bank_and_index_and_back) {
  for (unsigned int offset = 0; offset < 80; offset += 1) {
    ASSERT_EQ(offset, sensor_offset(sensor_bank(offset), sensor_index(offset)));
  }
}

TEST(TrackDataTest, test_sensor_may_be_seen_next) {
  track_state t;
  init_track(&t);

  // The pair of the current sensor
  EXPECT_TRUE(sensor_may_be_seen_next(&t, sensor_offset('A', 1), sensor_offset('A', 2)));
  EXPECT_TRUE(sensor_may_be_seen_next(&t, sensor_offset('B', 10), sensor_offset('B', 9)));
  EXPECT_TRUE(sensor_may_be_seen_next(&t, sensor_offset('B', 15), sensor_offset('B', 16)));

  // Two sequential sensors
  EXPECT_TRUE(sensor_may_be_seen_next(&t, sensor_offset('B', 9), sensor_offset('A', 5)));

  // The pair of the sensor ahead
  EXPECT_TRUE(sensor_may_be_seen_next(&t, sensor_offset('B', 9), sensor_offset('A', 6)));
  EXPECT_TRUE(sensor_may_be_seen_next(&t, sensor_offset('A', 7), sensor_offset('B', 11)));

  // The sensor two sensors ahead
  EXPECT_TRUE(sensor_may_be_seen_next(&t, sensor_offset('B', 9), sensor_offset('C', 7)));
  EXPECT_TRUE(sensor_may_be_seen_next(&t, sensor_offset('B', 3), sensor_offset('D', 2)));
  EXPECT_TRUE(sensor_may_be_seen_next(&t, sensor_offset('B', 16), sensor_offset('B', 3)));

  // The sensor behind in the opposite direction
  EXPECT_TRUE(sensor_may_be_seen_next(&t, sensor_offset('D', 14), sensor_offset('B', 2)));
  EXPECT_TRUE(sensor_may_be_seen_next(&t, sensor_offset('B', 10), sensor_offset('A', 5)));

  // The sensor two sensors behind in the opposite direction
  EXPECT_TRUE(sensor_may_be_seen_next(&t, sensor_offset('D', 2), sensor_offset('B', 4)));

  // Reversing around a corner
  EXPECT_TRUE(sensor_may_be_seen_next(&t, sensor_offset('B', 9), sensor_offset('A', 7)));
  EXPECT_TRUE(sensor_may_be_seen_next(&t, sensor_offset('B', 9), sensor_offset('A', 9)));
  EXPECT_TRUE(sensor_may_be_seen_next(&t, sensor_offset('B', 9), sensor_offset('A', 12)));
  EXPECT_FALSE(sensor_may_be_seen_next(&t, sensor_offset('B', 9), sensor_offset('A', 8)));
  EXPECT_FALSE(sensor_may_be_seen_next(&t, sensor_offset('B', 9), sensor_offset('A', 10)));
  EXPECT_FALSE(sensor_may_be_seen_next(&t, sensor_offset('B', 9), sensor_offset('A', 11)));

  EXPECT_TRUE(sensor_may_be_seen_next(&t, sensor_offset('C', 7), sensor_offset('C', 6)));

  // A sensor shouldn't be triggered twice in a row
  EXPECT_FALSE(sensor_may_be_seen_next(&t, sensor_offset('A', 1), sensor_offset('A', 1)));

  // Distant sensors shouldn't be triggered in a row
  EXPECT_FALSE(sensor_may_be_seen_next(&t, sensor_offset('B', 16), sensor_offset('C', 2)));
  EXPECT_FALSE(sensor_may_be_seen_next(&t, sensor_offset('A', 1), sensor_offset('C', 3)));
}

TEST(TrackDataTest, test_location_reverse) {
  track_state t;
  init_track(&t);

  location loc, result;

  loc = { .node = find_sensor(&t, sensor_offset('A', 1)), .offset = 0 };
  location_reverse(&result, &loc);
  EXPECT_EQ(find_sensor(&t, sensor_offset('A', 2)), result.node);
  EXPECT_EQ(0, result.offset);

  loc = { .node = find_sensor(&t, sensor_offset('B', 12)), .offset = 5 };
  location_reverse(&result, &loc);
  EXPECT_EQ(find_sensor(&t, sensor_offset('B', 11)), result.node);
  EXPECT_EQ(-5, result.offset);

  loc = { .node = find_sensor(&t, sensor_offset('E', 16)), .offset = -53 };
  location_reverse(&result, &loc);
  EXPECT_EQ(find_sensor(&t, sensor_offset('E', 15)), result.node);
  EXPECT_EQ(53, result.offset);
}

TEST(TrackDataTest, test_sensor_next) {
  track_state t;
  init_track(&t);

  turnout_state turnouts[NUM_TURNOUTS];
  init_turnouts(turnouts);

  EXPECT_EQ(find_sensor(&t, sensor_offset('A', 5)), sensor_next(find_sensor(&t, sensor_offset('B', 9)), turnouts));
  EXPECT_EQ(find_sensor(&t, sensor_offset('D', 14)), sensor_next(find_sensor(&t, sensor_offset('B', 1)), turnouts));
  EXPECT_EQ(find_sensor(&t, sensor_offset('C', 7)), sensor_next(find_sensor(&t, sensor_offset('A', 8)), turnouts));
  EXPECT_EQ(find_sensor(&t, sensor_offset('B', 3)), sensor_next(find_sensor(&t, sensor_offset('C', 10)), turnouts));

  // Dead ends
  EXPECT_EQ(NULL_TRACK_NODE, sensor_next(find_sensor(&t, sensor_offset('B', 8)), turnouts));
  EXPECT_EQ(NULL_TRACK_NODE, sensor_next(find_sensor(&t, sensor_offset('C', 3)), turnouts));

  // Four-way switch
  EXPECT_EQ(find_sensor(&t, sensor_offset('E', 2)), sensor_next(find_sensor(&t, sensor_offset('C', 2)), turnouts));
  EXPECT_EQ(find_sensor(&t, sensor_offset('E', 2)), sensor_next(find_sensor(&t, sensor_offset('B', 13)), turnouts));
  EXPECT_EQ(find_sensor(&t, sensor_offset('B', 14)), sensor_next(find_sensor(&t, sensor_offset('D', 1)), turnouts));
  EXPECT_EQ(find_sensor(&t, sensor_offset('B', 14)), sensor_next(find_sensor(&t, sensor_offset('E', 1)), turnouts));

  // Going straight over a single turnout
  turnouts[turnout_num_to_map_offset(16)] = TURNOUT_STRAIGHT;
  EXPECT_EQ(find_sensor(&t, sensor_offset('B', 1)), sensor_next(find_sensor(&t, sensor_offset('C', 10)), turnouts));

  // Four-way switch with two sequential switches straight
  turnouts[turnout_num_to_map_offset(153)] = TURNOUT_CURVED;
  turnouts[turnout_num_to_map_offset(154)] = TURNOUT_STRAIGHT;
  turnouts[turnout_num_to_map_offset(155)] = TURNOUT_STRAIGHT;
  turnouts[turnout_num_to_map_offset(156)] = TURNOUT_STRAIGHT;
  EXPECT_EQ(NULL_TRACK_NODE, sensor_next(find_sensor(&t, sensor_offset('C', 2)), turnouts));
  EXPECT_EQ(NULL_TRACK_NODE, sensor_next(find_sensor(&t, sensor_offset('B', 13)), turnouts));
  EXPECT_EQ(find_sensor(&t, sensor_offset('C', 1)), sensor_next(find_sensor(&t, sensor_offset('D', 1)), turnouts));
  EXPECT_EQ(find_sensor(&t, sensor_offset('C', 1)), sensor_next(find_sensor(&t, sensor_offset('E', 1)), turnouts));
}

void test_canonicalization(track_state *t, turnout_state turnouts[NUM_TURNOUTS],
                           char *node, int offset,
                           char *expected_node, int expected_offset) {
  location loc;
  loc.node = find_node_by_name(t, node);
  loc.offset = offset;

  location_canonicalize(turnouts, &loc, &loc);

  EXPECT_EQ(find_node_by_name(t, expected_node), loc.node);
  EXPECT_EQ(expected_offset, loc.offset);
}

TEST(TrackDataTest, test_location_canonicalize) {
  track_state t;
  init_track(&t);

  turnout_state turnouts[NUM_TURNOUTS];
  init_turnouts(turnouts);

  // Basic test
  test_canonicalization(&t, turnouts, "A5", 0, "A5", 0);

  // No next sensor
  test_canonicalization(&t, turnouts, "A2", 400 * 100, "A2", 400 * 100);

  // After next sensor, which does not have a next sensor
  turnouts[turnout_num_to_map_offset(11)] = TURNOUT_STRAIGHT;
  turnouts[turnout_num_to_map_offset(12)] = TURNOUT_STRAIGHT;
  test_canonicalization(&t, turnouts, "C14", (43 + 188 + 231 + 400) * 100, "A2", 400 * 100);

  // After branch
  test_canonicalization(&t, turnouts, "A3", (43 + 333 + 10) * 100, "C11", 10 * 100);

  // After multiple sensors
  test_canonicalization(&t, turnouts, "C7", 1200 * 100, "D10", 24 * 100);

  // Through four-way switch
  turnouts[turnout_num_to_map_offset(153)] = TURNOUT_CURVED;
  turnouts[turnout_num_to_map_offset(154)] = TURNOUT_STRAIGHT;
  test_canonicalization(&t, turnouts, "E3", 1300 * 100, "C9", 39 * 100);

  // To non-sensor node
  test_canonicalization(&t, turnouts, "E3", 600 * 100, "BR153", 153 * 100);

  // From non-sensor node
  test_canonicalization(&t, turnouts, "BR6", 1100 * 100, "E11", 153 * 100);
}

void expect_node_follows(track_state *t, char *start_name, char *end_name, bool expect) {
  track_node *start = find_node_by_name(t, start_name);
  ASSERT_NE(nullptr, start);

  track_node *end = find_node_by_name(t, end_name);
  ASSERT_NE(nullptr, end);

  EXPECT_EQ(expect, node_follows(start, end));
}

TEST(TrackDataTest, test_node_follows) {
  track_state t;
  init_track(&t);

  expect_node_follows(&t, "B8", "EX7", true);
  expect_node_follows(&t, "EN9", "B9", true);
  expect_node_follows(&t, "B9", "A5", true);
  expect_node_follows(&t, "A5", "MR3", true);
  expect_node_follows(&t, "MR3", "C7", true);
  expect_node_follows(&t, "C7", "MR18", true);
  expect_node_follows(&t, "MR18", "BR5", true);
  expect_node_follows(&t, "BR5", "C3", true);
  expect_node_follows(&t, "BR5", "MR7", true);

  expect_node_follows(&t, "BR16", "B1", true);

  expect_node_follows(&t, "B9", "A6", false);
  expect_node_follows(&t, "B9", "EX9", false);
  expect_node_follows(&t, "A5", "C7", false);
}

TEST(TrackDataTest, test_node_index_in_track_state) {
  track_state t;
  init_track(&t);

  track_node *start = find_sensor(&t, sensor_offset('B', 9));
  EXPECT_EQ(24, node_index_in_track_state(&t, start));
  EXPECT_EQ(4, node_index_in_track_state(&t, AHEAD(start)));
  EXPECT_EQ(85, node_index_in_track_state(&t, AHEAD(AHEAD(start))));

  for (int i = 0; i < TRACK_MAX; i += 1) {
    EXPECT_EQ(i, node_index_in_track_state(&t, &t.track[i]));
  }
}

#ifndef ALLTESTS
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif /* ALLTESTS */
