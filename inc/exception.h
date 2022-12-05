// See LICENSE file for copyright and license details.
#pragma once
#include <stdint.h>

#include "proc.h"
void exception_handler(uint64_t mcause, uint64_t mtval, uint64_t mepc);
