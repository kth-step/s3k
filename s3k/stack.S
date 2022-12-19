// See LICENSE file for copyright and license details.
#include "platform.h"

.globl stack

.section .bss.stack
.balign STACK_SIZE
.fill ((MAX_HARTID - MIN_HARTID + 1) * STACK_SIZE)
stack:
