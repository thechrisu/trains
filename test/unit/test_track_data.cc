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

#ifndef ALLTESTS
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif /* ALLTESTS */