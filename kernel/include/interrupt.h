#pragma once
#include <proc.h>
#include <stdint.h>

proc_t *interrupt_handler(proc_t *proc, uint64_t mcause, uint64_t mtval);
