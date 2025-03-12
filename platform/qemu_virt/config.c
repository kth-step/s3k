#include "cap/types.h"
#include "cap/util.h"
#include "drivers/uart.h"

const struct uart uart = UART_NS16550A(0x10000000);

struct uart *const kout = (struct uart *const)&uart;

cap_t cap_init(int i)
{
	cap_t init_caps[] = {
	    [0] = cap_mk_pmp(0x20005fff, MEM_RWX),
	    [1] = cap_mk_memory(0x80010000, 0x88000000, MEM_RWX),
	    [2] = cap_mk_memory(0x10000000, 0x10001000, MEM_RW),
	    [3] = cap_mk_memory(0x200b000, 0x200c000, MEM_R),
	    [4] = cap_mk_time(0, 0, NSLOT),
	    [5] = cap_mk_monitor(0, NPROC),
	    [6] = cap_mk_channel(0, NCHAN),
	    [7] = {.raw = 0},
	};

	return init_caps[i];
}
