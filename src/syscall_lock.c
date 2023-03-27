#include "ticket_lock.h"

// Lock used for capability operations.
static struct ticket_lock _lock;

void syscall_lock(void)
{
	tl_acq(&_lock);
}

void syscall_unlock(void)
{
	tl_rel(&_lock);
}
