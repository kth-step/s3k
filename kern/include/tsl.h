#ifndef TSL_H
#define TSL_H

#include "types.h"

/**
 * @struct tsl
 * @brief Represents a time slice capability with ownership, cfree, and time range.
 *
 * This structure defines a time slice capability, which includes information about the owner,
 * the amount of cfree (both remaining and initial), and the time range associated with the capability.
 */
typedef struct {
	pid_t owner;	  ///< Process ID of the owner of the capability.
	fuel_t cfree;	  ///< Remaining free for the capability.
	fuel_t csize;	  ///< Initial cfree allocated to the capability.
	hart_t hart;	  ///< The hart of the time slice capability.
	bool enabled;	  ///< If the time slots are used.
	time_slot_t base; ///< Start address of the time slots.
	time_slot_t size; ///< End address of the time slots.
	time_slot_t free; ///< Start of the allocated region.
} __attribute__((aligned(16))) tsl_t;

void tsl_init();

/**
 * Checks if the time slice capability is valid for the given owner and index.
 *
 * This function checks if the time slice capability at the given index is valid
 * for the specified owner process.
 *
 * @param owner The owner of the capability.
 * @param i The index of the capability.
 * @return true if valid, false otherwise.
 */
bool tsl_valid_access(pid_t owner, index_t i);

/**
 * Transfer a time slice capability from one process to another.
 *
 * @param owner The process ID of the current owner of the time slice capability.
 * @param index The index in the time table of the capability to be transferred.
 * @param new_owner The process ID of the new owner of the time slice capability.
 * @return ERR_SUCCESS if the capability is successfully granted,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the time table.
 */
int tsl_transfer(pid_t owner, index_t i, pid_t new_owner);

/**
 * Retrieves a time slice capability from the time table.
 *
 * @param owner The process ID associated with the time slice capability.
 * @param index The index in the time table.
 * @param cap A pointer to store the retrieved time slice capability.
 * @return ERR_SUCCESS if the time slice capability is successfully retrieved,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the time table.
 */
int tsl_get(pid_t owner, index_t i, tsl_t *cap);

/**
 * Derives a new time slice capability from an existing one.
 *
 * @param owner The process ID associated with the existing time slice capability.
 * @param index The index in the time table of the existing capability.
 * @param child_pid The process ID for the new capability.
 * @param child_fuel The amount of cfree for the new capability.
 * @param child_begin The start address of the new capability's time slots.
 * @param child_end The end address of the new capability's time slots.
 * @return The index of the new time slice capability if successfully derived,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the time table,
 *         ERR_INVALID_ARGUMENT if the new time slice capability cannot be derived.
 */
int tsl_derive(pid_t owner, index_t i, pid_t child_pid, fuel_t child_fuel, bool child_enabled, time_slot_t child_size);

/**
 * Revokes a time slice capability and its children.
 *
 * @param owner The process ID associated with the time slice capability.
 * @param index The index in the time table.
 * @return ERR_SUCCESS if the time slice capability is successfully revoked,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the time table.
 */
int tsl_revoke(pid_t owner, index_t i);

/**
 * Deletes a time slice capability by invalidating its process ID.
 *
 * @param owner The process ID associated with the time slice capability.
 * @param index The index in the time table.
 * @return ERR_SUCCESS if the time slice capability is successfully deleted,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the time table.
 */
int tsl_delete(pid_t owner, index_t i);

/**
 * Enables or disables a time slice capability in the scheduler.
 *
 * @param owner The process ID associated with the time slice capability.
 * @param index The index in the time table.
 * @param enable True to enable the time slice capability, false to disable it.
 * @return ERR_SUCCESS if the time slice capability is successfully enabled or disabled,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the time table.
 */
int tsl_set(pid_t owner, index_t i, bool enable);

#endif // TSL_H
