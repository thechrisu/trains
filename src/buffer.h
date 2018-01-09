//
// Created by Christoph Ulshöfer on 2018-01-04.
//

#ifndef TRAINS_BUFFER_H
#define TRAINS_BUFFER_H

#include "stdlib.h"

struct char_buffer {
  char *data;
  unsigned int elems;
  unsigned int size;
  unsigned int in;
  unsigned int out;
};

typedef struct char_buffer char_buffer;

struct str_buffer {
  char **data;
  unsigned int elems;
  unsigned int size;
  unsigned int in;
  unsigned int out;
  unsigned int elem_maxsize;
};

typedef struct str_buffer str_buffer;

char char_buffer_get(char_buffer *buf);
int char_buffer_put(char_buffer *buf, char c);
unsigned int char_buffer_get_num_elems(char_buffer *buf);
bool char_buffer_is_empty(char_buffer *buf);
bool char_buffer_is_full(char_buffer *buf);
void char_buffer_put_force(char_buffer *buf, char c);

char *str_buffer_get(str_buffer *buf);
int str_buffer_put(str_buffer *buf, char *str);
unsigned int str_buffer_get_num_elems(str_buffer *buf);
bool str_buffer_is_full(str_buffer *buf);

void char_buffer_init(char_buffer *buf, char *data, unsigned int size);
void char_buffer_empty(char_buffer *buf);
void str_buffer_init(str_buffer *buf, char **data, unsigned int size, unsigned int elem_maxsize);

#endif /* TRAINS_BUFFER_H */
