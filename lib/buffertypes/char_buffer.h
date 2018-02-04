/**
 * @file
 * @brief Declarations for character buffer.
 *        See the source in <code>lib/buffer.h</code>
 */
#ifndef CHAR_BUFFER_H
#define CHAR_BUFFER_H

#include "tstdlib.h"

typedef struct {
  uint32_t size, elems, in, out;
  char *data;
} char_buffer;

void char_buffer_init(char_buffer *b, char *d, uint32_t size);
void char_buffer_put(char_buffer *b, char e);
int char_buffer_put_safe(char_buffer *b, char e);
void char_buffer_put_replace(char_buffer *b, char e);
void char_buffer_remove(char_buffer *b);
char char_buffer_get(char_buffer *b);
void char_buffer_empty(char_buffer *b);
uint32_t char_buffer_length(char_buffer *b);
bool char_buffer_is_full(char_buffer *b);
bool char_buffer_is_empty(char_buffer *b);
uint32_t char_buffer_iter_prev_starti(char_buffer *b);
uint32_t char_buffer_iter_prev_endx(char_buffer *b);
uint32_t char_buffer_next_i(char_buffer *b, uint32_t i);
uint32_t char_buffer_prev_i(char_buffer *b, uint32_t i);

#endif /* CHAR_BUFFER_H */
