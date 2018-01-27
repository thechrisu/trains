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

#ifndef ALLTESTS
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif /* ALLTESTS */
