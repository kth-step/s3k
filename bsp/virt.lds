/* See LICENSE file for copyright and license details. */
OUTPUT_ARCH(riscv)
ENTRY(_start)

_kernel_base = 0x80000000;
_payload = _kernel_base + 0x10000;

SECTIONS {
	. = _kernel_base;
	.text : {
		*(.init)
		*(.text .text.*)
	}

	.data : {
		_data = .;
		*(.data .data.*)
		*(.rodata .rodata.*)
		_sdata = .;
		*(.sdata .sdata.*)
		*(.srodata .srodata.*)
	}

	.bss : {
		_bss = .;
		_sbss = .;
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		_end = ALIGN(8);
	}

	__global_pointer$ = MIN(_sdata + 0x800, MAX(_data + 0x800, _end - 0x800));

	ASSERT ((_end <= _payload), "ERROR: Kernel binary too large.")
}
