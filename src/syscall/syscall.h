//
// Created by Christoph Ulshöfer on 2018-01-15.
//

#ifndef TRAINS_SYSCALL_H
#define TRAINS_SYSCALL_H

void syscall_create(int priority, void (*code)());
int syscall_mytid();
int syscall_myparent_tid();
void syscall_pass();
void syscall_exit();

#endif /* TRAINS_SYSCALL_H */
