#pragma once

#include "mcslock.h"

#include <stdbool.h>
#include <stdint.h>

void kernel_init(void);
bool kernel_lock(void);
void kernel_unlock(void);
