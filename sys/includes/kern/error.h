#pragma once

typedef enum {
	SUCCESS = 0,
	ERR_EMPTY,
	ERR_EXISTS,
	ERR_INVARG,
	ERR_INVDRV,
	ERR_INVCAP,
	ERR_INVSTATE,
	ERR_PREEMPT,
	ERR_TIMEOUT,
	ERR_SUSPENDED,
	CONTINUE = -1, // Special for revocation
} Error;
