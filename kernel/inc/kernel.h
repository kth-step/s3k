#pragma once

#include "proc.h"

#include <stdbool.h>
#include <stdint.h>

void kernel_init(void);
uint64_t kernel_wcet(void);
void kernel_wcet_reset(void);

bool kernel_lock_acquire(void);
void kernel_lock_release(void);

void kernel_preempt_enable(void);
void kernel_preempt_disable(void);
bool kernel_preempt(void);
