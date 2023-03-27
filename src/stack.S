/* See LICENSE file for copyright and license details. */
#include "stack.h"
.globl stack_top

.section .bss.stack

stack:
.space (STACK_SIZE * NHART) 
stack_top:
