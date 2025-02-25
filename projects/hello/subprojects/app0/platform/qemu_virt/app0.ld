/* See LICENSE file for copyright and license details. */
OUTPUT_ARCH(riscv)
ENTRY(_start)

_payload = ORIGIN(RAM) + LENGTH(RAM);

__global_pointer$ = MIN(_sdata + 0x800, MAX(_data + 0x800, _end - 0x800));

MEMORY {
	RAM (rwx) : ORIGIN = 0x80010000, LENGTH = 64K
}

SECTIONS {
	.text : {
		*( .text.init )
		*( .text .text.* )
	} > RAM

	.data : ALIGN(4K) {
		_data = . ;
		*( .data )
		*( .data.* )
		_sdata = . ;
		*( .sdata )
		*( .sdata.* )
	} > RAM

	.bss : ALIGN(4K) {
		_bss = .;
		_sbss = .;
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		_end = ALIGN(8);
	} > RAM
}
