/**
 * @file proc.h
 * @brief Process control block.
 *
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 * @bug QEMU mret does not work properly if all pmp registers are 0, so there is a workaround in
 * sched_next
 */
#pragma once
#include <stdint.h>

#include "common.h"

void sched_yield(void);
void sched_next(void);
void sched_init(void);

void sched_delete(uint64_t hartid, uint64_t begin, uint64_t end);
void sched_update(uint64_t hartid, uint64_t pid, uint64_t begin, uint64_t end);
