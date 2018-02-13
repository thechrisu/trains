#include "test_stdlib.h"
#include <gtest/gtest.h>

using namespace std;

#ifndef ALLTESTS
int oe_in_sensor = false;
#endif /* ALLTESTS */

TEST(Stdlib_Test, test_strlen) {
  ASSERT_EQ(tstrlen(""), 0);
  ASSERT_EQ(tstrlen("test"), 4);
  char test[6] = {'t', 'e', 's', 't', '\0', '\0'};
  ASSERT_EQ(strlen(test), 4);
  test[2] = '\0';
  ASSERT_EQ(strlen(test), 2);
}

TEST(Stdlib_Test, test_memcpy_happypath) {
  char test[] = "TestStr";
  char *rcv = (char *)malloc(tstrlen(test));
  tmemcpy((void *)rcv, (void *)test, tstrlen(test));
  for (int i = 0; i < tstrlen(test); i++) {
    ASSERT_EQ(test[i], rcv[i]);
  }
}

TEST(Stdlib_Test, test_memcpy_toofew) {
  char test[] = "TestStr";
  char test_rcv[] = "TestSt";
  char *rcv = (char *)malloc(tstrlen(test));
  tmemcpy((void *)rcv, (void *)test, tstrlen(test) - 1);
  for (int i = 0; i < tstrlen(test_rcv); i++) {
    ASSERT_EQ(test_rcv[i], rcv[i]);
  }
}

TEST(Stdlib_Test, test_memset) {
  int length = 10;
  char WallE[] = "TERMINATOR";
  tmemset(WallE, 'W', 5);
  tmemset(WallE + 5, 'E', 5);
  string WallEString(WallE, length);
  ASSERT_EQ(WallEString, "WWWWWEEEEE");
}

TEST(Stdlib_Test, test_strcmp) {
  ASSERT_TRUE(tstrcmp("", ""));
  ASSERT_TRUE(tstrcmp("test1", "test1"));
  ASSERT_FALSE(tstrcmp("k1", "k12"));
  ASSERT_FALSE(tstrcmp("q", "q1"));
}

TEST(Stdlib_Test, test_strncmp) {
  ASSERT_EQ(tstrncmp("", "", 10), 0);
  ASSERT_EQ(tstrncmp("k1", "k1", 10), 0);
  ASSERT_EQ(tstrncmp("k1", "k", 10), 1);
  ASSERT_EQ(tstrncmp("k", "k1", 10), -1);
  ASSERT_EQ(tstrncmp("k1", "k2", 10), -1);
  ASSERT_EQ(tstrncmp("k2", "k1", 10), 1);
  ASSERT_EQ(tstrncmp("k2", "k1", 1), 0);
}

TEST(Stdlib_Test, test_a2d) {
  ASSERT_EQ(-1, a2d('\0'));
  ASSERT_EQ(-1, a2d('g'));
  for (int i = 0; i < 10; i += 1) {
    ASSERT_EQ(i, a2d('0' + i));
  }
  for (int i = 0; i < 6; i += 1) {
    ASSERT_EQ(i + 10, a2d('A' + i));
    ASSERT_EQ(i + 10, a2d('a' + i));
  }
}

TEST(Stdlib_Test, test_a2i) {
  char c;
  char source[] = "0 123 444 DEADBEEF D";
  char *p = source;
  int result;

  c = *p;
  p += 1;
  c = a2i(c, &p, 10, &result);
  ASSERT_EQ(' ', c);
  ASSERT_EQ(&(source[2]), p);
  ASSERT_EQ(0, result);

  c = *p;
  p += 1;
  c = a2i(c, &p, 10, &result);
  ASSERT_EQ(' ', c);
  ASSERT_EQ(&(source[6]), p);
  ASSERT_EQ(123, result);

  c = *p;
  p += 1;
  c = a2i(c, &p, 8, &result);
  ASSERT_EQ(' ', c);
  ASSERT_EQ(&(source[10]), p);
  ASSERT_EQ(292, result);

  c = *p;
  p += 1;
  c = a2i(c, &p, 16, &result);
  ASSERT_EQ(' ', c);
  ASSERT_EQ(&(source[19]), p);
  ASSERT_EQ(0xDEADBEEF, result);

  c = *p;
  p += 1;
  c = a2i(c, &p, 10, &result);
  ASSERT_EQ('D', c);
  ASSERT_EQ(&(source[20]), p);
  ASSERT_EQ(0, result);
}

TEST(Stdlib_Test, test_ui2a) {
  char buf[16];

  ui2a(0, 10, buf);
  ASSERT_STREQ("0", buf);

  ui2a(12345, 10, buf);
  ASSERT_STREQ("12345", buf);

  ui2a(0x12345678, 16, buf);
  ASSERT_STREQ("12345678", buf);

  ui2a(0xDEADBEEF, 16, buf);
  ASSERT_STREQ("deadbeef", buf);

  ui2a(-1, 16, buf);
  ASSERT_STREQ("ffffffff", buf);
}

TEST(Stdlib_Test, test_i2a) {
  char buf[16];

  i2a(0, buf);
  ASSERT_STREQ("0", buf);

  i2a(12345, buf);
  ASSERT_STREQ("12345", buf);

  i2a(-12345, buf);
  ASSERT_STREQ("-12345", buf);
}

#ifndef ALLTESTS
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif /* ALLTESTS */
