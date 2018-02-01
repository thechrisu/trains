#include "idle.h"

void idle_task() {
  RegisterAs("Idle");
  while(1);
  Assert(0);
}
