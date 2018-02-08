#ifndef IO_H
#define IO_H

#if TESTING
#else
typedef char *va_list;

#define __va_argsiz(t)                                          \
  (((sizeof(t) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#define va_start(ap, pN) ((ap) = ((va_list) __builtin_next_arg(pN)))

#define va_end(ap)    ((void)0)

#define va_arg(ap, t)                                                   \
  (((ap) = (ap) + __va_argsiz(t)), *((t*) (void*) ((ap) - __va_argsiz(t))))

#endif /* TESTING */

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
