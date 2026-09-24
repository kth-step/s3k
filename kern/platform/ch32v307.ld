OUTPUT_ARCH(riscv) /* Specify the target architecture. */
ENTRY(_start)      /* Define the entry point of the kernel. */

/* CH32V307 has no CLINT/mtime. The RTC backend is the SysTick-style free
 * counter described in ch32v307.c (32-bit low word of the STK CNT, an UP
 * counter at HCLK/8, read from the STK/CPPSR region at 0xE000F000). rtc.c
 * on RV32 expects a two-word __mtime[] (lo, hi) and a two-word
 * __mtimecmp[][2]; the storage is REAL .bss arrays defined in ch32v307.c.
 * Small-data sections (.sdata/.sbss) are captured explicitly in
 * .data/.bss below so no gp-relative symbol can escape the kernel window. */

MEMORY {
    /* Kernel CODE executes from flash (M-mode is not PMP-checked, so it
     * needs no capability window; the factory program occupies only the
     * first KBs of flash). Its writable data+bss plus the stack live in a
     * 4K SRAM window at the very top, ABOVE the three partition NAPOT
     * regions:
     *   app1 16K @0x20000000, app3 16K @0x20004000, app2 32K @0x20008000.
     * The 0x800 scratch block at 0x20000000 is reserved for the mtime
     * aliases; the partition budget is enforced by the capability system
     * in ch32v307.c, not by this linker region. */
    FLASH (rx) : ORIGIN = 0x08008000, LENGTH = 0x10000
    RAM (rwx) : ORIGIN = 0x2000F000, LENGTH = 0x1000 /* to SRAM end 0x20010000 */
}

SECTIONS {
    /* Code section */
    .text : {
        *(.text.init)       /* Initialization code. */
        *(.text .text.*)    /* Main code. */
    } > FLASH

    /* Read-only data stays in flash next to the code. */
    .rodata : ALIGN (8) {
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
    } > FLASH

    /* Data section: VMA in SRAM, LMA in flash (loader copies). */
    .data ORIGIN(RAM) : AT(LOADADDR(.rodata) + SIZEOF(.rodata)) {
        _data = .;          /* Start of the data section. */
        *(.data .data.*)    /* Initialized data. */
        *(.sdata .sdata.*)  /* gp-relative initialized data. */
    } > RAM

    /* BSS section */
    .bss : ALIGN (8) {
        _bss = .;           /* Start of uninitialized data. */
        *(.bss .bss.*)      /* Uninitialized data. */
        *(.sbss .sbss.*)    /* gp-relative uninitialized data. */
    } > RAM
    _end = ALIGN(8);    /* End of allocated sections. */
    /* Flash address where the .data image (LMA) lives; head.S copies it
     * to the SRAM VMA before zeroing .bss. */
    _data_lma = LOADADDR(.data);

    /* Global pointer and stack */
    __global_pointer$ = ORIGIN(RAM) + LENGTH(RAM) / 2;
    __stack_top = ORIGIN(RAM) + LENGTH(RAM); /* Define the top of the stack. */
}
