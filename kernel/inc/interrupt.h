#pragma once
#include <proc.h>
#include <stdint.h>

proc_t *interrupt_handler(proc_t *proc, val_t mcause, val_t mtval);
