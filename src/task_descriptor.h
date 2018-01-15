#ifndef TASK_DESCRIPTOR_H
#define TASK_DESCRIPTOR_H

typedef struct {
  int priority;
  struct td *prev;
  struct td *next;
} task_descriptor;

#endif /* TASK_DESCRIPTOR_H */
