#include "test_buffer.h"
#include <gtest/gtest.h>
#include <vector>

#define BUF_SIZE 10

using namespace std;

#ifndef ALLTESTS
int oe_in_sensor = false;
#endif /* ALLTESTS */

TEST(BufferTest, should_print_correctly) {
  char buf_data[BUF_SIZE];
  char_buffer buf;
  char_buffer_init(&buf, buf_data, BUF_SIZE);
  ASSERT_FALSE(char_buffer_is_full(&buf));
  ASSERT_TRUE(char_buffer_is_empty(&buf));
  for (int i = 0; i < BUF_SIZE; i++) {
    ASSERT_EQ(char_buffer_length(&buf), i < BUF_SIZE ? i : BUF_SIZE);
    ASSERT_FALSE(char_buffer_is_full(&buf));
    char_buffer_put(&buf, i);
  }
  ASSERT_TRUE(char_buffer_is_full(&buf));
  ASSERT_FALSE(char_buffer_is_empty(&buf));
  ASSERT_EQ(char_buffer_length(&buf), BUF_SIZE);
  for (int i = 0; i < BUF_SIZE; i++) {
    char_buffer_get(&buf);
  }
  ASSERT_FALSE(char_buffer_is_full(&buf));
  ASSERT_TRUE(char_buffer_is_empty(&buf));
}

TEST(BufferTest, put_replace) {
  char buf_data[BUF_SIZE];
  char_buffer buf;
  char_buffer_init(&buf, buf_data, BUF_SIZE);
  for (int i = 0; i < 2 * BUF_SIZE; i++) {
    char_buffer_put_replace(&buf, 'A' + i);
  }
  ASSERT_TRUE(char_buffer_is_full(&buf));
  ASSERT_FALSE(char_buffer_is_empty(&buf));
  ASSERT_EQ(char_buffer_length(&buf), BUF_SIZE);
  for (int i = 0; i < BUF_SIZE; i++) {
    ASSERT_EQ(char_buffer_get(&buf), 'A' + i + BUF_SIZE);
  }
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

TEST(BufferTest, iterate_backwards) {
  char buf_data[BUF_SIZE];
  char_buffer buf;
  char_buffer_init(&buf, buf_data, BUF_SIZE);
  char_buffer_put(&buf, 'A');
  ASSERT_EQ(char_buffer_iter_prev_starti(&buf), 0);
  ASSERT_EQ(char_buffer_prev_i(&buf, 0), 0);

  char_buffer_put(&buf, 'B');
  ASSERT_EQ(char_buffer_iter_prev_starti(&buf), 1);
  ASSERT_EQ(char_buffer_prev_i(&buf, 1), 0);
  ASSERT_EQ(char_buffer_prev_i(&buf, 0), 1);

  for (int i = 0; i < 8; i++) {
    char_buffer_put(&buf, 'C' + i);
  }
  ASSERT_EQ(char_buffer_iter_prev_starti(&buf), 9);
  ASSERT_EQ(char_buffer_prev_i(&buf, 9), 8);
  ASSERT_EQ(char_buffer_prev_i(&buf, 1), 0);
  ASSERT_EQ(char_buffer_prev_i(&buf, 0), 9);
}

TEST(BufferTest, put_replace_iterate_backwards) {
  char buf_data[BUF_SIZE];
  char_buffer buf;
  char_buffer_init(&buf, buf_data, BUF_SIZE);
  for (int i = 0; i < 15; i++) {
    char_buffer_put_replace(&buf, 'A' + i);
  }

  ASSERT_EQ(char_buffer_iter_prev_starti(&buf), 4);
  ASSERT_EQ(char_buffer_prev_i(&buf, 4), 3);
  ASSERT_EQ(char_buffer_prev_i(&buf, 3), 2);
  ASSERT_EQ(char_buffer_prev_i(&buf, 0), 9);
  ASSERT_EQ(char_buffer_prev_i(&buf, 6), 5);
  ASSERT_EQ(char_buffer_prev_i(&buf, 5), 4);
}

TEST(BufferTest, iterate_backwards_empty) {
  char buf_data[BUF_SIZE];
  char_buffer buf;
  char_buffer_init(&buf, buf_data, BUF_SIZE);
  ASSERT_EQ(char_buffer_prev_i(&buf, 0), 0);
  ASSERT_EQ(char_buffer_iter_prev_starti(&buf), 0);
}

#ifndef ALLTESTS
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#endif /* ALLTESTS */
