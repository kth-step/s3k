#pragma once

#include "kern/types.h"

void syscall_handler(reg_t a0, reg_t a1, reg_t a2, reg_t a3,
		     reg_t a4, reg_t a5, reg_t a6, reg_t a7);
