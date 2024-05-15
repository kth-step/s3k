.globl _sp

.section .bss.stack
.balign 8
.skip S3K_HART_CNT * (1 << S3K_LOG_STACK_SIZE)
_sp:
