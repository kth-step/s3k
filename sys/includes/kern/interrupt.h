#pragma once

#include "kern/types.h"

void interrupt_handler(uint64_t mcause, uint64_t mtval);
