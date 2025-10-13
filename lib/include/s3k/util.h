#ifndef S3K_UTIL_H
#define S3K_UTIL_H

#include "s3k/types.h"

static inline s3k_word_t s3k_pmp_napot_encode(s3k_word_t base, s3k_word_t size)
{
	return (base | (size / 2 - 1)) >> 2;
}

static inline s3k_word_t s3k_pmp_napot_decode_base(s3k_word_t addr)
{
	return ((addr + 1) & addr) << 2;
}

static inline s3k_word_t s3k_pmp_napot_decode_size(uint64_t addr)
{
	return (((addr + 1) ^ addr) + 1) << 2;
}

#endif // S3K_UTIL_H
