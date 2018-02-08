/**
 * @file
 * @brief Busy-wait I/O functions, buffered + unbuffered.
 */
#ifndef BWIO_H
#define BWIO_H

#include "assert.h"
#include "char_buffer.h"
#include "io.h"
#include "mytimer.h"
#include "tstdlib.h"

#define IOBUFFERSZ 20000

extern int oe_in_sensor;

void setup_buffered_io();
int trysendall();
int tryreceiveall();
int trysendbyte(int channel); // TODO docs
int tryreceivebyte(int channel);
int sendbyte_buffered(int channel, char c);
bool canreadbyte_buffered(int channel);
char readbyte_buffered(int channel);
uint16_t getnumreadable_bytes(int channel);
void empty_buf(int channel, bool out);
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

#if !VERSATILEPB && (HOSTCONFIG || TESTING)

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

#endif /* VERSATILEPB && (HOSTCONFIG || TESTING) */

#endif /* BWIO_H */
