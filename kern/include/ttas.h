#ifndef TTAS_H
#define TTAS_H

#include "types.h"

typedef struct ttas {
	volatile word_t lock;
} ttas_t;

/**
 * Initialize the TTAS lock.
 */
void ttas_init(ttas_t *ttas);

/**
 * Acquires the TTAS lock.
 */
bool ttas_acquire(ttas_t *ttas, bool preemptable);

/**
 * Releases the TTAS lock.
 */
void ttas_release(ttas_t *ttas);

#endif
