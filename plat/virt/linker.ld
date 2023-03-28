/* See LICENSE file for copyright and license details. */
OUTPUT_ARCH(riscv)
ENTRY(_start)

CODE_ALIGNMENT = 4; /* Align code on a 4 byte boundary */
DATA_ALIGNMENT = 8; /* Align data on a 8 byte boundary */

_mtime = 0x200bff8;
_mtimecmp = 0x2004000;

__global_pointer$ = MIN(_sdata + 0x800, MAX(_data + 0x800, _end - 0x800));

__payload = ORIGIN(RAM) + LENGTH(RAM);

MEMORY {
	RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 64K
}

SECTIONS {
	.text : ALIGN(CODE_ALIGNMENT) {
		*( .text.init )
		*( .text .text.* )
	} > RAM

	.data : ALIGN(DATA_ALIGNMENT) {
		_data = . ;
		*( .data )
		*( .data.* )
		_sdata = . ;
		*( .sdata )
		*( .sdata.* )
	} > RAM

	.bss : ALIGN(DATA_ALIGNMENT) {
		_bss = .;
		_sbss = .;
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		_end = ALIGN(DATA_ALIGNMENT);
	} > RAM
}
