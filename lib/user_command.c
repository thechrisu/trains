#include "user_command.h"

void user_command_reset(user_command *cmd) {
  cmd->type = NULL_USER_CMD;
  cmd->data[0] = 0;
  cmd->data[1] = 0;
  cmd->data[2] = 0;
}
