.globl _sp

.section .bss.stack
.balign 256
.skip NHART * 256
_sp:
