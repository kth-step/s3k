#pragma once
#include <stdint.h>
#include <proc.h>

proc_t *interrupt_handler(proc_t *proc, uint64_t mcause, uint64_t mtval);
