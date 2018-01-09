//
// Created by Christoph Ulshöfer on 2018-01-06.
//

#include "test_buffer.h"
#include <gtest/gtest.h>
#include <vector>

#define BUF_SIZE 10

int oe_in_sensor = false;

TEST(BufferTest, should_print_correctly) {
  char buf_data[BUF_SIZE];
  char_buffer buf;
  char_buffer_init(&buf, buf_data, BUF_SIZE);
  init_track(&global_track_state);
  for (int i = 0; i < 7; i++) {
    char_buffer_put(&buf, i);
  }
  for (int i = 0; i < 5; i++) {
    if (char_buffer_is_full(&buf)) {
        char_buffer_get(&buf);
    }
    char_buffer_put(&buf, i + 32);
  }
  print_triggered_sensors(&buf);
}

TEST(BufferTest, buffer_should_work_i_am_sorry_i_dont_have_more_detail) {
  char buf_data[2 * BUF_SIZE];
  char_buffer buf;
  char_buffer_init(&buf, buf_data, 2 * BUF_SIZE);
  for (int j = 0; j < 10; j++) {
    std::vector<char> inDataVec, bufferGetVec;
  for (int i = 0; i < 10; i++) {
    char_buffer_put(&buf, i);
    ASSERT_EQ(buf.in, (i+1 + (j%2) * 10) % 20);
  }
  for (int i = 0; i < 10; i++) {
    inDataVec.push_back(buf.data[i]);
  }
  for (int i = 0; i < 10; i++) {
    bufferGetVec.push_back(char_buffer_get(&buf));
    ASSERT_EQ(buf.out, (i+1 + (j%2) * 10) % 20);

  }
  ASSERT_TRUE(std::equal(inDataVec.begin(), inDataVec.end(), bufferGetVec.begin()));
  ASSERT_EQ(buf.in, buf.out);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
