#include "rtc.h"

#if __riscv_xlen != 32 && __riscv_xlen != 64
#error "Unsupported RISC-V architecture. Only 32-bit and 64-bit are supported."
#endif

// External variables for RTC time and timeout registers.
#if __riscv_xlen == 64
extern volatile uint64_t __mtime[];
extern volatile uint64_t __mtimecmp[];

/**
 * @brief Get the current RTC time (64-bit architecture).
 * @return Current time in ticks.
 */
uint64_t rtc_get_time(void)
{
	return __mtime[0];
}

/**
 * @brief Set the RTC time (64-bit architecture).
 * @param time Time in ticks to set.
 */
void rtc_set_time(uint64_t time)
{
	__mtime[0] = time;
}

/**
 * @brief Get the timeout value for a specific hart (64-bit architecture).
 * @param hartid ID of the hardware thread.
 * @return Timeout value in ticks.
 */
uint64_t rtc_get_timeout(word_t hartid)
{
	return __mtimecmp[hartid];
}

/**
 * @brief Set the timeout value for a specific hart (64-bit architecture).
 * @param hartid ID of the hardware thread.
 * @param time Timeout value in ticks to set.
 */
void rtc_set_timeout(word_t hartid, uint64_t time)
{
	__mtimecmp[hartid] = time;
}

#elif __riscv_xlen == 32
extern volatile uint32_t __mtime[];
extern volatile uint32_t __mtimecmp[][2];

/**
 * @brief Get the current RTC time (32-bit architecture).
 * @return Current time in ticks as a 64-bit value.
 */
uint64_t rtc_get_time(void)
{
	uint32_t hi_time, lo_time;
	do {
		hi_time = __mtime[1];
		lo_time = __mtime[0];
	} while (hi_time != __mtime[1]); // Ensure atomic read.
	return ((uint64_t)hi_time << 32) | lo_time;
}

/**
 * @brief Set the RTC time (32-bit architecture).
 * @param time Time in ticks to set.
 */
void rtc_set_time(uint64_t time)
{
	__mtime[0] = 0;			     // Clear low 32 bits.
	__mtime[1] = (uint32_t)(time >> 32); // Set high 32 bits.
	__mtime[0] = (uint32_t)time;	     // Set low 32 bits.
}

/**
 * @brief Get the timeout value for a specific hart (32-bit architecture).
 * @param hartid ID of the hardware thread.
 * @return Timeout value in ticks as a 64-bit value.
 */
uint64_t rtc_get_timeout(word_t hartid)
{
	uint32_t lo_time = __mtimecmp[hartid][0];
	uint32_t hi_time = __mtimecmp[hartid][1];
	return ((uint64_t)hi_time << 32) | lo_time;
}

/**
 * @brief Set the timeout value for a specific hart (32-bit architecture).
 * @param hartid ID of the hardware thread.
 * @param time Timeout value in ticks to set.
 */
void rtc_set_timeout(word_t hartid, uint64_t time)
{
	__mtimecmp[hartid][0] = (uint32_t)time;		// Set low 32 bits.
	__mtimecmp[hartid][1] = (uint32_t)(time >> 32); // Set high 32 bits.
}

#endif
