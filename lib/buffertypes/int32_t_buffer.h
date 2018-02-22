/**
 * @file
 * @brief Declarations for int32_t buffer.
 *        See the source in <code>lib/buffer.h</code>
 */
#ifndef INT32_T_BUFFER_H
#define INT32_T_BUFFER_H

#include "tstdlib.h"

typedef struct {
  uint32_t size, elems, in, out;
  int32_t *data;
} int32_t_buffer;

void int32_t_buffer_init(int32_t_buffer *b, int32_t *d, uint32_t size);
void int32_t_buffer_put(int32_t_buffer *b, int32_t e);
int int32_t_buffer_put_safe(int32_t_buffer *b, int32_t e);
void int32_t_buffer_put_replace(int32_t_buffer *b, int32_t e);
void int32_t_buffer_remove(int32_t_buffer *b);
int32_t int32_t_buffer_peek(int32_t_buffer *b);
int32_t int32_t_buffer_get(int32_t_buffer *b);
void int32_t_buffer_empty(int32_t_buffer *b);
uint32_t int32_t_buffer_length(int32_t_buffer *b);
bool int32_t_buffer_is_full(int32_t_buffer *b);
bool int32_t_buffer_is_empty(int32_t_buffer *b);
uint32_t int32_t_buffer_iter_prev_starti(int32_t_buffer *b);
uint32_t int32_t_buffer_iter_prev_endx(int32_t_buffer *b);
uint32_t int32_t_buffer_next_i(int32_t_buffer *b, uint32_t i);
uint32_t int32_t_buffer_prev_i(int32_t_buffer *b, uint32_t i);

#endif /* INT32_T_BUFFER_H */
