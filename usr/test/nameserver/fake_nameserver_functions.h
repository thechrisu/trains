#ifndef TRAINS_FAKE_NAMESERVER_FUNCTIONS_H
#define TRAINS_FAKE_NAMESERVER_FUNCTIONS_H

#include "stdlib.h"
#include "../../../include/common/codes.h"

extern int ns_tid;

char send_msg(char *c, char msg_type);
char fake_RegisterAs(char *c);
char fake_WhoIs(char *c);
char fake_Kill();
#endif /* TRAINS_FAKE_NAMESERVER_FUNCTIONS_H */
