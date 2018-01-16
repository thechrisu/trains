//
// Created by Christoph Ulshöfer on 2018-01-15.
//

#ifndef TRAINS_SYSCALL_H
#define TRAINS_SYSCALL_H

#include "../multitasking/schedule.h"
#include "../multitasking/task.h"

extern void enter_main();

/**
 * Kernel-level implementation of creating and registering a user task.
 *
 * @param priority Priority of task (>=0)
 * @param code     Entrypoint of task
 * @return         >0: tid of created task,
 *                 -1: if priority was invalid,
 *                 -2: if the maximum number of tasks have been registered already.
 */
int syscall_create(int priority, void (*code)());
int syscall_mytid();
int syscall_myparent_tid();
void syscall_pass();
void syscall_exit();

#endif /* TRAINS_SYSCALL_H */
