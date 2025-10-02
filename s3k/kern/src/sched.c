#include "sched.h"

#include "csr.h"
#include "lock.h"
#include "rtc.h"

extern void temporal_fence(void);

// Structure representing a scheduling frame (time slot) for a process
typedef struct frame {
	uint16_t pid;    // Process ID assigned to this slot
	uint16_t length; // Length of the slot in time units
} frame_t;

// Scheduling table: for each hart (hardware thread), an array of frames
frame_t schedule[_NUM_HARTS][MAX_TIME_SLOT];

// Current slot index for each hart
uint64_t curr[_NUM_HARTS];

/**
 * Returns the current global scheduling slot based on the RTC.
 */
uint64_t sched_rtc_slot()
{
	return (rtc_get_time() >> 10);
}

/**
 * Converts a slot index to an absolute time value.
 */
uint64_t slot2time(uint64_t slot)
{
	return (slot << 10);
}

/**
 * Initializes the scheduler:
 * - Sets up the initial schedule for each hart.
 * - Assigns the first slot to PID 1 on hart 0, INVALID_PID elsewhere.
 * - Resets the current slot and RTC.
 */
void sched_init(void)
{
	for (int hart = 0; hart < _NUM_HARTS; hart++) {
		schedule[hart][0].pid = (hart == 0) ? 1 : INVALID_PID;
		schedule[hart][0].length = MAX_TIME_SLOT;
		curr[hart] = 0;
	}
	rtc_set_time(0);
}

/**
 * Reclaims a range of scheduling slots [begin, end) for a specific process.
 * If the current slot is within this range, updates curr to begin.
 */
void sched_reclaim(hart_t hart, pid_t pid, time_slot_t begin, time_slot_t end)
{
	schedule[hart][begin].pid = pid;
	schedule[hart][begin].length = end - begin;

	// If the current slot is within the reclaimed range, update it to begin
	uint64_t curr_local = curr[hart] % MAX_TIME_SLOT;
	if (begin <= curr_local && curr_local < end) {
		curr[hart] += begin - curr_local;
	}
}

/**
 * Splits a frame into two parts:
 * - [begin, middle): old PID
 * - [middle, end): new PID
 * If middle == begin, just sets the PID for the slot.
 */
void sched_split(hart_t hart, pid_t pid, time_slot_t begin, time_slot_t middle, time_slot_t end)
{
	schedule[hart][begin].length = middle - begin;
	schedule[hart][middle].pid = pid;
	schedule[hart][middle].length = end - middle;

	uint64_t curr_local = curr[hart] % MAX_TIME_SLOT;
	if (curr_local == begin) {
		// If currently at 'begin', possibly advance to 'middle'
		middle = curr[hart] + middle - begin;
		if (middle < sched_rtc_slot()) {
			curr[hart] = middle;
		}
	}
}

/**
 * Sets the PID for a specific slot on a hart.
 * Used when enabling/disabling tsl capabilities.
 */
void sched_set_pid(hart_t hart, pid_t pid, time_slot_t begin)
{
	schedule[hart][begin].pid = pid;
}

/**
 * Retrieves the next process to run for a given hart.
 * Advances the current slot if needed, checks for valid and ready processes.
 * Sets the timeout for the next scheduling event.
 */
static proc_t *sched_next(hart_t hart, uint64_t *timeout)
{
	bool swapped = false;

	// Lock because other processes may be accessing the schedule
	lock_acquire(false);
	uint64_t rtc_slot = sched_rtc_slot();
	uint64_t offset = curr[hart] % MAX_TIME_SLOT;
	// Advance curr if the current slot has expired
	if (curr[hart] + schedule[hart][offset].length <= rtc_slot) {
		curr[hart] += schedule[hart][offset].length;
		swapped = true;
	}
	frame_t slot = schedule[hart][curr[hart] % MAX_TIME_SLOT];
	*timeout = slot2time(curr[hart] + slot.length);
	lock_release();
	// Release lock because we do not want to block when executing temporal fence.

	if (swapped) {
		temporal_fence(); // Insert a temporal fence if we swapped slots
	}

	if (slot.pid == INVALID_PID) {
		return NULL; // No process scheduled for this slot
	}

	// We now have a process we may schedule.
	proc_t *proc = proc_get(slot.pid);
	if (proc->timeout > slot2time(rtc_slot)) {
		return NULL; // Process is sleeping or waiting
	}

	// Try to acquire the process
	lock_acquire(false);
	bool proc_acquired = proc_acquire(slot.pid);
	proc->timeout = *timeout;
	lock_release();
	return proc_acquired ? proc : NULL;
}

/**
 * Main scheduler function.
 * Loops until a ready process is found, otherwise waits for an interrupt.
 */
proc_t *sched(void)
{
	hart_t hart = csrr_mhartid();
	uint64_t timeout;

	while (1) {
		proc_t *next = sched_next(hart, &timeout);
		rtc_set_timeout(hart, timeout);

		if (next != NULL) {
			return next; // Return the next ready process
		}

		// Wait for interrupt if no process is ready
		while (!(csrr_mip() & 128)) {
			__asm__ volatile("wfi");
		}
	}
}
