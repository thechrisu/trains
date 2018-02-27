#include "sensor_interpreter.h"

void sensor_interpreter() {
  int sender_tid;
  message received;

  RegisterAs("SensorInterpreter");

  while (1) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));
    Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
  }
}
