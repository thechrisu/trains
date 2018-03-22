#include "user_command.h"

void user_command_reset(user_command *cmd) {
  cmd->type = NULL_USER_CMD;
  for (int i = 0; i < 10; i += 1) {
    cmd->data[i] = 0;
  }
}
