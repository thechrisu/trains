/**
 * @file
 * @brief Generic definitions of I/O primitives.
 */
#ifndef IO_H
#define IO_H

#include "format.h"

#define ON      1
#define OFF     0

#if VERSATILEPB
#include "../kernel/include/versatilepb/versatilepb.h"
#define TRAIN 0
#define TERMINAL 1

#else

#if !(HOSTCONFIG || TESTING)
#include "../kernel/include/labenv/ts7200.h"
#endif /* HOSTCONFIG || TESTING */

#define TRAIN 1
#define TERMINAL 0

#endif /* VERSATILEPB */

#endif /* IO_H */
