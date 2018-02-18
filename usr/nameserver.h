/**
 * @file
 * @brief Nameserver task implementation.
 */
#ifndef TRAINS_NAMESERVER_H
#define TRAINS_NAMESERVER_H

#include "tstdlib.h"
#include "codes.h"
#include "messages.h"


#define NAMESERVER_MSG_LENGTH 32

#if E2ETESTING
  #define TOTAL_NUM_NAMES 64
#else
  #define TOTAL_NUM_NAMES 512
#endif /* E2ETESTING */


/**
 * Runs the nameserver. All R and W messages expect a \0 at the end.
 * For messages to the nameserver, the first letter indicates the type of message to send.
 * R: RegisterAs. In this case, the next characters will be treated as the task name.
 * W: WhoIs. In this case, the next characters will be treated as the name to be queried.
 * K: Kill. Stops the nameserver.
 *
 * Message return values:
 * In general, a single char is sent as a reply.
 * If the value of this character is >= 128, it indicates an error.
 * You can get the error type by subtracting 128 and interpreting the result as an ASCII character:
 * Error types:
 * T: Truncated. The name supplied was too long, it won't register.
 * S: Short. You did not supply enough characters.
 * M: Many. You called RegisterAs too many times.
 * N: Not found. Your WhoIs did not match any name.
 * W: Wrong. The first letter of your message was neight R nor W nor K.
 *
 * Successful return values:
 * RegisterAs returns a single character 'C' on success.
 * WhoIs returns a single character, which is to be interpreted as the tid of the task belonging to the name.
 *
 * To register a task, send a string "Rtaskname", where
 */
void nameserver_main();

#endif /* TRAINS_NAMESERVER_H */
