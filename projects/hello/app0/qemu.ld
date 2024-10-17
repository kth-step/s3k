/* See LICENSE file for copyright and license details. */
OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY {
	RAM (rwx) : ORIGIN = 0x80010000, LENGTH = 0x10000
}

__stack_size = 1024;
__global_pointer$ = MIN(_sdata + 0x800, MAX(_data + 0x800, _end - 0x800));

SECTIONS {
	.text : {
		*( .init )
		*( .text .text.* )
	} > RAM

	.data : {
		_data = . ;
		*( .data )
		*( .data.* )
		_sdata = . ;
		*( .sdata )
		*( .sdata.* )
	} > RAM

	.bss : {
		_bss = .;
		_sbss = .;
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		_end = .;
	} > RAM

	.stack : ALIGN(8) {
		. += __stack_size;
		__stack_pointer = .;
		_end = .;
	}
}
