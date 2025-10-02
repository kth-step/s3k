#ifndef RTC_H
#define RTC_H

#include "types.h"

#define RTC_TIMEOUT_MAX 0xFFFFFFFFFFFFFFFF // Maximum timeout value for the RTC

/**
 * @brief Get the current RTC time.
 *
 * @return Current time as a 64-bit unsigned integer.
 */
uint64_t rtc_get_time(void);

/**
 * @brief Set the current RTC time.
 *
 * @param time Time to set, as a 64-bit unsigned integer.
 */
void rtc_set_time(uint64_t time);

/**
 * @brief Get the timeout value for a specific hardware thread (hart).
 *
 * @param hartid ID of the hardware thread.
 * @return Timeout value as a 64-bit unsigned integer.
 */
uint64_t rtc_get_timeout(word_t hartid);

/**
 * @brief Set the timeout value for a specific hardware thread (hart).
 *
 * @param hartid ID of the hardware thread.
 * @param time Timeout value to set, as a 64-bit unsigned integer.
 */
void rtc_set_timeout(word_t hartid, uint64_t time);

#endif // RTC_H
