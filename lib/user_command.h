/**
 * @file
 * @brief User commands.
 */
#ifndef USER_COMMAND_H
#define USER_COMMAND_H

enum user_command_type {
  USER_CMD_GO,
  USER_CMD_STOP,
  USER_CMD_TR,
  USER_CMD_SW,
  USER_CMD_RV,
  USER_CMD_Q,
  USER_CMD_SD,
  USER_CMD_V,
  USER_COMMAND_LOOP,
  USER_CMD_SET,
  USER_CMD_R,
  NULL_USER_CMD
};

typedef struct {
  enum user_command_type type;
  int data[3];
} user_command;

/**
 * Initializes/resets a user_command struct.
 */
void user_command_reset(user_command *cmd);

#endif /* USER_COMMAND_H */
