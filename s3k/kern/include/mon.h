#ifndef CAP_MON_H
#define CAP_MON_H
#include "proc.h"
#include "types.h"

typedef struct {
	pid_t owner;  ///< Process ID of the owner of the capability.
	fuel_t cfree; ///< Remaining cfree for the capability.
	fuel_t csize; ///< Initial cfree allocated to the capability.
	pid_t pid;    ///< The process ID of controlled process.
} __attribute__((aligned(sizeof(word_t)))) mon_t;

void mon_init();

/**
 * Checks if the monitor capability is valid for the given owner and index.
 *
 * This function checks if the monitor capability at the given index is valid
 * for the specified owner process.
 *
 * @param owner The owner of the capability.
 * @param i The index of the capability.
 * @return true if valid, false otherwise.
 */
bool mon_valid_access(pid_t owner, index_t i);

/**
 * Transfers a monitor capability from one process to another.
 *
 * @param owner The process ID of the current owner of the monitor capability.
 * @param index The index in the monitor table of the capability to be transferred.
 * @param new_owner The process ID of the new owner of the monitor capability.
 * @return ERR_SUCCESS if the capability is successfully granted,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the monitor table.
 */
int mon_transfer(pid_t owner, index_t i, pid_t new_owner);

/**
 * Retrieves a monitor capability from the monitor table.
 *
 * @param owner The process ID associated with the monitor capability.
 * @param index The index in the monitor table.
 * @param cap_monitor A pointer to store the retrieved monitor capability.
 * @return ERR_SUCCESS if the monitor capability is successfully retrieved,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the monitor table.
 */
int mon_get(pid_t owner, index_t i, mon_t *cap_monitor);

/**
 * Derives a new monitor capability from an existing one.
 *
 * @param owner The process ID associated with the existing monitor capability.
 * @param index The index in the monitor table of the existing capability.
 * @param child_pid The process ID of owner for the new capability.
 * @param child_fuel The amount of cfree for the new capability.
 * @return The index of the new monitor capability if successfully derived,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the monitor table,
 *         ERR_INVALID_ARGUMENT if the new monitor capability cannot be derived.
 */
int mon_derive(pid_t owner, index_t i, pid_t child_pid, fuel_t child_fuel);

/**
 * Revokes a monitor capability and its derived capabilities.
 *
 * @note This function invalidates the capability and all its children.
 *
 * @param owner The process ID associated with the monitor capability.
 * @param index The index in the monitor table of the capability to be revoked.
 * @return ERR_SUCCESS if the capability is successfully revoked,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the monitor table.
 */
int mon_revoke(pid_t owner, index_t i);

/**
 * Deletes a monitor capability.
 *
 * @param owner The process ID associated with the monitor capability.
 * @param index The index in the monitor table of the capability to be deleted.
 * @return ERR_SUCCESS if the capability is successfully deleted,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the monitor table.
 */
int mon_delete(pid_t owner, index_t i);

pid_t mon_get_pid(pid_t owner, index_t i);

/**
 * Suspends a process.
 *
 * @param owner The process ID associated with the monitor capability.
 * @param index The index in the monitor table of the capability to be suspended.
 * @return ERR_SUCCESS if the process is successfully suspended,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the monitor table.
 */
int mon_suspend(pid_t owner, index_t i);

/**
 * Resumes a process.
 *
 * @param owner The process ID associated with the monitor capability.
 * @param index The index in the monitor table of the capability to be resumed.
 * @return ERR_SUCCESS if the process is successfully resumed,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the monitor table.
 */
int mon_resume(pid_t owner, index_t i);

/**
 * Yields the current time to the monitored process.
 *
 * @param owner The process ID of the owner of the capability.
 * @param index The index in the monitor table of the capability to be yielded.
 * @param next A pointer to store the next process to run.
 *             This is set to the process that is being yielded to.
 * @return ERR_SUCCESS if the time is successfully yielded,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the monitor table,
 *         ERR_INVALID_STATE if the process is not in a valid state to yield time.
 */
int mon_yield(pid_t owner, index_t i, proc_t **next);

/**
 * Set the register value for the monitored process.
 */
int mon_reg_set(pid_t owner, index_t i, word_t reg, word_t value);

/**
 * Get the register value for the monitored process.
 */
int mon_reg_get(pid_t owner, index_t i, word_t reg, word_t *value);

#endif // CAP_MON_H
