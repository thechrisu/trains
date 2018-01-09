//
// Created by Christoph Ulshöfer on 2018-01-04.
//

#include "buffer.h"

void char_buffer_init(char_buffer *buf, char *data, unsigned int size) {
  buf->data = data;
  buf->size = size;
  buf->in = 0;
  buf->out = 0;
  buf->elems = 0;
}

void str_buffer_init(str_buffer *buf, char **data, unsigned int size, unsigned int elem_maxsize) {
  buf->data = data;
  buf->size = size;
  buf->in = 0;
  buf->out = 0;
  buf->elems = 0;
  buf->elem_maxsize = elem_maxsize;
}

int char_buffer_put(char_buffer *buf, char c) {
  if (buf->elems < buf->size) {
    buf->data[buf->in] = c;
    buf->in += 1;
    if (buf->in >= buf->size) {
      buf->in = 0;
    }
    buf->elems++;
    return 0;
  } else {
    return 1;
  }
}

void char_buffer_put_force(char_buffer *buf, char c) {
  buf->data[buf->in] = c;
  buf->in += 1;
  if (buf->in >= buf->size) {
    buf->in = 0;
  }
  buf->elems++;
}

unsigned int char_buffer_get_num_elems(char_buffer *buf) {
  return buf->elems;
}

bool char_buffer_is_empty(char_buffer *buf) {
  return buf->elems == 0;
}

bool char_buffer_is_full(char_buffer *buf) {
  return buf->elems == buf->size;
}

char char_buffer_get(char_buffer *buf) {
  if (buf->elems > 0) {
    char ret = buf->data[buf->out];
    buf->out += 1;
    if (buf->out >= buf->size) {
      buf->out = 0;
    }
    buf->elems--;
    return ret;
  }
  return 0;
}

void char_buffer_empty(char_buffer *buf) {
  buf->elems = 0;
  buf->in = 0;
  buf->out = 0;
}

int str_buffer_put(str_buffer *buf, char *c) {
  if (buf->elems < buf->size) {
    for (unsigned int i = 0; i <= buf->elem_maxsize; i++) {
      buf->data[buf->in][i] = c[i];
      if (c[i] == '\0') {
        break;
      }
    }
    buf->data[buf->in][buf->elem_maxsize] = '\0'; // TODO fix issue when pressing enter
    buf->in += 1;
    if (buf->in >= buf->size) {
      buf->in = 0;
    }
    buf->elems++;
    return 0;
  } else {
    return 1;
  }
}

unsigned int str_buffer_get_num_elems(str_buffer *buf) {
  return buf->elems;
}

bool str_buffer_is_full(str_buffer *buf) {
  return buf->elems == buf->size;
}

char *str_buffer_get(str_buffer *buf) {
  if (buf->elems > 0) {
    char *ret = buf->data[buf->out];
    buf->out += 1;
    if (buf->out >= buf->size) {
      buf->out = 0;
    }
    buf->elems--;
    return ret;
  }
  return 0;
}
