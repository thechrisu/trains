#include <gtest/gtest.h>
#include "test_all.h"

int oe_in_sensor = false;
extern task_descriptor *current_task;

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
