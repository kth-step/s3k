OUTPUT_ARCH(riscv) /* Specify the target architecture. */
ENTRY(_start)      /* Define the entry point of the kernel. */

__mtime      = 0x0204bff8; /* Address for the machine timer. */
__mtimecmp   = 0x02044000; /* Address for the machine timer compare. */

MEMORY {
    RAM (rwx) : ORIGIN = 0x10000000, LENGTH = 0x10000 /* Define the RAM region. */
}

SECTIONS {
    /* Code section */
    .text : {
        *(.text.init)       /* Initialization code. */
        *(.text .text.*)    /* Main code. */
    } > RAM

    /* Data section */
    .data : ALIGN (8) {
        _data = .;          /* Start of the data section. */
        *(.data .data.*)    /* Initialized data. */
    } > RAM

    /* BSS section */
    .bss : ALIGN (8) {
        _bss = .;           /* Start of uninitialized data. */
        *(.bss .bss.*)      /* Uninitialized data. */
    } > RAM
    _end = ALIGN(8);    /* End of allocated sections. */

    /* Global pointer and stack */
    __global_pointer$ = _data + 0x400;
    __stack_top = ORIGIN(RAM) + LENGTH(RAM); /* Define the top of the stack. */
}
