#ifndef TRAIN_SLEEPER_H
#define TRAIN_SLEEPER_H

#include "codes.h"
#include "messages.h"

/**
 * Task that takes a message containing the number of ticks it should sleep,
 * before sending a WAKEUP message.
 */
void sleeper();

/**
 * Creates a sleeper task that will send us a message of type MESSAGE_WAKEUP
 * after <code>ticks</code> have passed.
 *
 * @param ticks     Number of ticks to wait before sleeper will notfy us.
 */
void set_alarm(int ticks);

#endif /* TRAIN_SLEEPER_H */
