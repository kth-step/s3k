#pragma once

#include "proc.h"

#include <stdbool.h>
#include <stdint.h>

void kernel_init(void);
uint64_t kernel_wcet(void);
uint64_t kernel_wcet_reset(void);

void kernel_syscall_entry(void);
void kernel_syscall_exit(void);
bool kernel_preempt(void);
