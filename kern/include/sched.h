#ifndef SCHED_H
#define SCHED_H

#include "proc.h"
#include "types.h"

void sched_init(void);

/**
 * @brief Reclaims a range of scheduling slots for a specific process.
 *
 * Updates the scheduling table to assign the specified range of slots
 * to the given process. If the current slot falls within the reclaimed
 * range, it updates the current slot to the start of the range.
 *
 * @param hart The hardware thread ID (hart) to reclaim slots for.
 * @param pid The process ID to assign the slots to.
 * @param begin The starting index of the slot range.
 * @param end The ending index of the slot range.
 */
void sched_reclaim(hart_t hart, pid_t pid, time_slot_t begin, time_slot_t end);

/**
 * @brief Splits a scheduling slot into two parts.
 *
 * Divides a slot into two segments, assigning the first segment to the
 * original process and creating a new slot for the second segment.
 * Adjusts the current slot if it falls within the second segment.
 *
 * @param hart The hardware thread ID (hart) to split the slot for.
 * @param pid The process ID of the slot to split.
 * @param begin The starting index of the slot.
 * @param mid The index where the slot is split.
 * @param end The ending index of the slot.
 */
void sched_split(hart_t hart, pid_t pid, time_slot_t begin, time_slot_t mid, time_slot_t end);

/**
 * @brief Sets the process ID for a specific scheduling slot.
 *
 * Updates the process ID for the specified slot in the scheduling table.
 * May set pid to INVALID_PID to disable the slot.
 *
 * @param hart The hardware thread ID (hart) to set the slot for.
 * @param pid The process ID to assign to the slot.
 * @param begin The index of the slot to update.
 */
void sched_set_pid(hart_t hart, pid_t pid, time_slot_t begin);

/**
 * @brief Main scheduler function to determine the next process to run.
 *
 * Continuously retrieves the next process until a valid process is found.
 *
 * @return A pointer to the next process to run.
 */
proc_t *sched(void);

#endif // SCHED_H
