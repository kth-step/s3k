#pragma once

#include <stdint.h>

/** Get the RT clock */
uint64_t time_get(void);

/** Set the RT clock */
void time_set(uint64_t time);

/** Get the RT clock timeout for hart `hartid' */
uint64_t timeout_get(uint64_t hartid);

/** Set the RT clock timeout for hart `hartid' */
void timeout_set(uint64_t hartid, uint64_t timeout);
