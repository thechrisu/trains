#include "idle.h"

void idle_task() {
  Assert(RegisterAs("Idle") == 0);
  while(1);
  Assert(0);
}
