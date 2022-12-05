// See LICENSE file for copyright and license details.
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "lock.h"
#include "proc.h"

#define INVALID_PID 0xFFull

void sched_init(void);
void sched_yield(void) __attribute__((noreturn));
void sched_start(void) __attribute__((noreturn));
void sched_update(cap_node_t* cn, uint64_t hartid, uint64_t begin, uint64_t end, uint64_t pid);
