/**
 * @file
 * @brief Mock implementation of RegisterAs/WhoIs syscalls, used to test nameserver functions.
 */
#ifndef TRAINS_FAKE_NAMESERVER_FUNCTIONS_H
#define TRAINS_FAKE_NAMESERVER_FUNCTIONS_H

#include "stdlib.h"
#include "../../../include/common/codes.h"

extern int ns_tid;

/**
 * Underlying, protocol-level implementation for fake_RegisterAs and fake_WhoIs.
 * @param c          String to send to the nameserver.
 * @param msg_type   R for RegisterAs, W for WhoIs.
 * @return Return from the nameserver.
 */
char send_msg(char *c, char msg_type);

/**
 * Mock implementation of RegisterAs.
 * @param c
 * @return Return from the nameserver.
 */
char fake_RegisterAs(char *c);

/**
 * Mock implementation of WhoIs.
 * @param c
 * @return Return from the nameserver.
 */
char fake_WhoIs(char *c);

/**
 * Backdoor to kill the nameserver.
 * @return Return from the nameserver.
 */
char fake_Kill();
#endif /* TRAINS_FAKE_NAMESERVER_FUNCTIONS_H */
