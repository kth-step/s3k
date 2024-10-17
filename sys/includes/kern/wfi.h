#pragma once

/**
 * @file wfi.h
 * @brief Declares wfi function.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */

static inline void wfi(void)
{
	__asm__ volatile("wfi");
}
