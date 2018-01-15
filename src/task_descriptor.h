#ifndef TASK_DESCRIPTOR_H
#define TASK_DESCRIPTOR_H

struct td {
  int priority;
  struct td *prev;
  struct td *next;
};

typedef struct td task_descriptor;

#endif /* TASK_DESCRIPTOR_H */
