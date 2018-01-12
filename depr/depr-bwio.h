/*
 * bwio.h
 */

#ifndef BWIO_H
#define BWIO_H

#include "../include/kernel/labenv/ts7200.h"
#include "../include/kernel/glue/mytimer.h"
#include "../src/stdlib.h"
#include "../src/buffer.h"
#include "../test-resources/assert.h"

extern int oe_in_sensor;

#if TESTING
#else
typedef char *va_list;

#define __va_argsiz(t)    \
        (((sizeof(t) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#define va_start(ap, pN) ((ap) = ((va_list) __builtin_next_arg(pN)))

#define va_end(ap)    ((void)0)

#define va_arg(ap, t)    \
         (((ap) = (ap) + __va_argsiz(t)), *((t*) (void*) ((ap) - __va_argsiz(t))))

#endif

#define ON      1
#define OFF     0

#define IOBUFFERSZ 20000

/**
 * Input: TO the ARM box
 * Output: FROM the ARM box
 */
char_buffer train_input_buf, train_output_buf,
    terminal_input_buf, terminal_output_buf;

char train_input_chars[IOBUFFERSZ], train_output_chars[IOBUFFERSZ],
    terminal_input_chars[IOBUFFERSZ], terminal_output_chars[IOBUFFERSZ];


void setup_io();
int trysendall();
int tryreceiveall();
int bwtrysendbyte(int channel); // TODO docs
int bwtryreceivebyte(int channel);
int bwsendbyte_buffered(int channel, char c);
bool bwcanreadbyte_buffered(int channel);
char bwreadbyte_buffered(int channel);
uint16_t bwgetnumreadable_bytes(int channel);
void empty_buf(int channel, bool out);
/////////////////////////////////////////////////////////////////////////

int bwsetfifo(int channel, int state);

int bwsetspeed(int channel, int speed);

int bwputc(int channel, char c);

int bwcanputc(int channel);

/**
 *
 * @param channel
 * @return 1 if you can get char, 0 if not, -1 on error
 */
int bwcangetc(int channel);

int bwgetc(int channel);

int bwputx(int channel, char c);

int bwputstr(int channel, char *str);

int bwputr(int channel, unsigned int reg);

void bwputw(int channel, int n, char fc, char *bf);

void bwprintf(int channel, char *format, ...);

#endif /* BWIO_H */
