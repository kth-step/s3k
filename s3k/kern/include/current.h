#ifndef CURRENT_H
#define CURRENT_H

#include "proc.h"
register proc_t *current __asm__("tp");

#endif // CURRENT_H
