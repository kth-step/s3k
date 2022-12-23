/* See LICENSE file for copyright and license details. */
OUTPUT_ARCH("riscv")

ENTRY(_start)
        
_payload = 0x80010000;
_kernel_base = 0x80000000;

SECTIONS {
        . = _kernel_base;
        .text : {
                _text = .;
                KEEP(*(.init))
                *(.text .text.*)
                _etext = .;
        }

        .data : ALIGN(4K) {
                _data = .;
                *(.data .data.*)
                *(.rodata .rodata.*)
                *(COMMON)
                 _sdata = .;
                *(.sdata .sdata.*)
                 *(.srodata .srodata*)
                 . = ALIGN(8);
                _edata = .;
        }

        .bss : ALIGN(8) {
                _bss = .;
                _sbss = .;
                *(.sbss .sbss.*)
                *(.bss .bss.*)
                . = ALIGN(8);
                _ebss = .;
        } 

        __global_pointer$ = MIN(_sdata + 0x800, MAX(_data + 0x800, _ebss - 0x800));
        
        ASSERT ((_ebss <= _payload), "Error: Kernel binary is too large.")
}
