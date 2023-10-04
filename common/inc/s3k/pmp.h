#pragma once

#include "s3k/types.h"

void s3k_napot_decode(s3k_napot_t napot_addr, s3k_addr_t *begin,
		      s3k_addr_t *end);
s3k_napot_t s3k_napot_encode(s3k_addr_t base, s3k_addr_t size);
