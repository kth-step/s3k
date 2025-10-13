#ifndef SYSCALL_H
#define SYSCALL_H

#include "proc.h"
#include "types.h"

/**
 * System call handler.
 * Returns the process that should be run next.
 * Returns NULL if the scheduler should be invoked.
 */
proc_t *syscall_handler(void);

#endif // SYSCALL_H
