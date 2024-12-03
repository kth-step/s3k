#pragma once

#include <stdint.h>
#include "s3k/types.h"

/** Get the RT clock */
s3k_time_t time_get(void);

/** Set the RT clock */
void time_set(s3k_time_t time);

/** Get the RT clock timeout for hart `hartid' */
s3k_time_t timeout_get(s3k_hart_t hartid);

/** Set the RT clock timeout for hart `hartid' */
void timeout_set(s3k_hart_t hartid, s3k_time_t timeout);
