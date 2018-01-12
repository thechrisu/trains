#ifndef MY_IO_H
#define MY_IO_H

#include "labenv/ts7200.h"

#if (HOSTCONFIG || TESTING)

#include <stdio.h>
#include <stdlib.h>
#define bwsendbyte_buffered(com, val) printf("%d: %c", com, val)
#define bwreadbyte_buffered(com) ('a' + com)
#define bwgetnumreadable_bytes(com) com
#define bwputc(com, val) printf("%d: %c", com, val)
#define empty_buf(com, outin) printf("%d: %c", com, outin + '0')
#define t_printf(...) printf(__VA_ARGS__)

#else

#include "labenv/bwio.h"
#define t_printf(...) bwprintf(COM1, __VA_ARGS__)
#define printf(...) bwprintf (COM2, __VA_ARGS__)
#define putc(c) bwputc(COM2, c)

#endif

#endif /* MY_IO_H */
