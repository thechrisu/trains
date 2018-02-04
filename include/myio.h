/**
 * @file
 * @brief I/O (wrapper) functions
 */
#ifndef MY_IO_H
#define MY_IO_H

#include "assert.h"
#include "char_buffer.h"
#include "mytimer.h"
#include "tstdlib.h"

#define LOG_BUFFER_SIZE 1<<22

extern int oe_in_sensor;

extern char log_buffer[LOG_BUFFER_SIZE];
extern uint32_t log_index;

#if TESTING
#else
typedef char *va_list;

#define __va_argsiz(t)    \
        (((sizeof(t) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#define va_start(ap, pN) ((ap) = ((va_list) __builtin_next_arg(pN)))

#define va_end(ap)    ((void)0)

#define va_arg(ap, t)    \
         (((ap) = (ap) + __va_argsiz(t)), *((t*) (void*) ((ap) - __va_argsiz(t))))

#endif /* TESTING */

#define ON      1
#define OFF     0

#define IOBUFFERSZ 20000

/**
 * Input: TO the ARM box
 * Output: FROM the ARM box
 */

void setup_io();
int trysendall();
int tryreceiveall();
int trysendbyte(int channel); // TODO docs
int tryreceivebyte(int channel);
int sendbyte_buffered(int channel, char c);
bool canreadbyte_buffered(int channel);
char readbyte_buffered(int channel);
uint16_t getnumreadable_bytes(int channel);
void empty_buf(int channel, bool out);
/////////////////////////////////////////////////////////////////////////
int setfifo(int channel, int state);
int setspeed(int channel, int speed);
int putc(int channel, char c);
int canputc(int channel);
/**
 *
 * @param channel
 * @return 1 if you can get char, 0 if not, -1 on error
 */
int cangetc(int channel);
int getc(int channel);
int bwgetc(int channel);
void printf(char *format, ...) __attribute((format(printf, 1, 0)));
void bwprintf(char *format, ...) __attribute((format(printf, 1, 0)));
void logprintf(char *format, ...) __attribute((format(printf, 1, 0)));
int logputc(char c);
int logputr(unsigned int reg);
void dump_logs();



#if VERSATILEPB
#include "../kernel/include/versatilepb/versatilepb.h"
#define TRAIN 0
#define TERMINAL 1

#else

#if (HOSTCONFIG || TESTING)

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define sendbyte_buffered(com, val) printf("%d: %c", com, val)
#define readbyte_buffered(com) ('a' + com)
#define getnumreadable_bytes(com) com

#undef putc
#define putc(com, val) printf("%d: %c", com, val)

#define empty_buf(com, outin) printf("%d: %c", com, outin + '0')
#define bwprintf printf
#else
#include "../kernel/include/labenv/ts7200.h"

#endif /* HOSTCONFIG || TESTING */

#define TRAIN 1
#define TERMINAL 0

#endif /* VERSATILEPB */


#endif /* MY_IO_H */
