#ifndef TASK_DESCRIPTOR_H
#define TASK_DESCRIPTOR_H

#define NULL_TASK_DESCRIPTOR (task_descriptor *)0

struct td {
  int priority;
  struct td *prev;
  struct td *next;
};

typedef struct td task_descriptor;

#endif /* TASK_DESCRIPTOR_H */
