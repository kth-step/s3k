#pragma once
#include <stdint.h>

uint64_t time_get(void);
void time_set(uint64_t);
uint64_t timeout_get(uintptr_t);
void timeout_set(uintptr_t, uint64_t);
