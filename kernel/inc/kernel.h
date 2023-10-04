#pragma once

#include "proc.h"

#include <stdbool.h>
#include <stdint.h>

void kernel_init(void);

bool kernel_lock(proc_t *p);
void kernel_unlock(proc_t *p);

void kernel_hook_sys_entry(proc_t *p);
void kernel_hook_sys_exit(proc_t *p);
