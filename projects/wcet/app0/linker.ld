/* See LICENSE file for copyright and license details. */
OUTPUT_ARCH(riscv)
ENTRY(_start)

__global_pointer$ = MIN(_sdata + 0x800, MAX(_data + 0x800, _end - 0x800));

MEMORY {
	RAM (rwx) : ORIGIN = 0x80010000, LENGTH = 64K
}

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
}
