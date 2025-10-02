OUTPUT_ARCH(riscv) /* Specify the target architecture. */
ENTRY(_start)      /* Define the entry point of the kernel. */

__uart_base  = 0x03002000; /* Base address for UART. */

MEMORY {
    RAM (rwx) : ORIGIN = 0x80100000, LENGTH = 512K /* Define the RAM region. */
}

SECTIONS {
    /* Code section */
    .text : {
        *(.text.init)       /* Initialization code. */
        *(.text .text.*)    /* Main code. */
    } > RAM

    /* Data section */
    .data : {
        _data = .;          /* Start of the data section. */
        *(.data .data.*)    /* Initialized data. */
        _sdata = .;         /* Start of small data section. */
        *(.sdata .sdata.*)  /* Small initialized data. */
    } > RAM

    /* BSS section */
    .bss : ALIGN (8){
        _bss = .;           /* Start of uninitialized data. */
        _sbss = .;          /* Start of the BSS section. */
        *(.sbss .sbss.*)    /* Small uninitialized data. */
        *(.bss .bss.*)      /* Uninitialized data. */
    } > RAM
    _end = ALIGN(8);    /* End of allocated sections. */

    /* Global pointer and stack */
    __global_pointer$ = MIN(_sdata + 0x800, MAX(_sdata + 0x800, _end - 0x800));
}
