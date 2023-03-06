#include "wfi.h"

void wfi()
{
	__asm__ volatile("wfi");
}
