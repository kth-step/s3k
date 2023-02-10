#pragma once
#include "common.h"

void trap_entry(void) __attribute__((noreturn));
void trap_exit(void) __attribute__((noreturn));
