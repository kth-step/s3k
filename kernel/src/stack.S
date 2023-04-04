.globl _sp

.section .bss.stack
.skip N_HART * (1 << LOG_STACK_SIZE)
_sp:
