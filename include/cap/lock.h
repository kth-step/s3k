#include <stdbool.h>

void cap_lock_init(void);
bool cap_lock_acquire(void);
void cap_lock_release(void);
