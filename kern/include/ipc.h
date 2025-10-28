#pragma once

#include "proc.h"
#include "types.h"

/**
 * @struct ipc
 * @brief Represents an IPC capability.
 */
typedef struct {
	pid_t owner;
	fuel_t cfree;
	fuel_t csize;
	ipc_mode_t mode : 2;
	ipc_flag_t flag : 5;
	index_t sink : 32;
	index_t source : 32;
} __attribute__((aligned(16))) ipc_t;

_Static_assert(sizeof(ipc_t) == 16, "IPC capability has the wrong size.");

void ipc_init();

/**
 * @brief Initializes the IPC capability system.
 *
 * This function sets up the IPC capability system, including initializing
 * the capability table and any necessary data structures.
 */
void ipc_init(void);

/**
 * @brief Checks if the IPC capability is valid for the given owner and index.
 *
 * This function checks if the IPC capability at the given index is valid
 * for the specified owner process.
 *
 * @param owner The owner of the capability.
 * @param i The index of the capability.
 * @return true if valid, false otherwise.
 */
bool ipc_valid_access(pid_t owner, index_t i);

/**
 * @brief Transfers ownership of an IPC capability.
 *
 * Ensures that the capability is moved from one process to another.
 *
 * @param owner The current owner of the capability.
 * @param i The index of the capability.
 * @param new_owner The new owner of the capability.
 * @return ERR_SUCCESS on success, or an error code on failure.
 */
int ipc_transfer(pid_t owner, index_t i, pid_t new_owner);

/**
 * @brief Retrieves an IPC capability.
 * @param owner The owner of the capability.
 * @param i The index of the capability.
 * @param cap Pointer to store the retrieved capability.
 * @return ERR_SUCCESS on success, or an error code on failure.
 */
int ipc_get(pid_t owner, index_t i, ipc_t *cap);

/**
 * @brief Derives a new IPC capability.
 * @param owner The owner of the parent capability.
 * @param i The index of the parent capability.
 * @param pid The owner of the derived capability.
 * @param cfree The cfree for the derived capability.
 * @param mode The mode of the derived capability.
 * @param flag The flags for the derived capability.
 * @return The index of the derived capability, or an error code on failure.
 */
int ipc_derive(pid_t owner, index_t i, pid_t pid, fuel_t cfree, ipc_mode_t mode, ipc_flag_t flag);

/**
 * @brief Revokes an IPC capability.
 * @param owner The owner of the capability.
 * @param i The index of the capability.
 * @return The number of revoked capabilities, or an error code on failure.
 */
int ipc_revoke(pid_t owner, index_t i);

/**
 * @brief Deletes an IPC capability.
 * @param owner The owner of the capability.
 * @param i The index of the capability.
 * @return ERR_SUCCESS on success, or an error code on failure.
 */
int ipc_delete(pid_t owner, index_t i);

/**
 * @brief Sends data and a capability to another process.
 * @param owner The owner of the IPC capability.
 * @param i The index of the IPC capability.
 * @param data The data to send.
 * @param capty The type of capability to send.
 * @param j The index of the capability to send.
 * @param next Pointer to store the next process to run.
 * @return ERR_SUCCESS on success, or an error code on failure.
 */
int ipc_send(pid_t owner, index_t i, word_t data[2], capty_t capty, index_t j, proc_t **next);

/**
 * @brief Receives data and a capability from another process.
 * @param owner The owner of the IPC capability.
 * @param i The index of the IPC capability.
 * @param next Pointer to store the next process to run.
 * @return ERR_SUCCESS on success, or an error code on failure.
 */
int ipc_recv(pid_t owner, index_t i, proc_t **next);

/**
 * @brief Calls a function in another process and waits for a reply.
 * @param owner The owner of the IPC capability.
 * @param i The index of the IPC capability.
 * @param data The data to send.
 * @param capty The type of capability to send.
 * @param j The index of the capability to send.
 * @param next Pointer to store the next process to run.
 * @return ERR_SUCCESS on success, or an error code on failure.
 */
int ipc_call(pid_t owner, index_t i, word_t data[2], capty_t capty, index_t j, proc_t **next);

/**
 * @brief Replies to a received message and sends a capability.
 * @param owner The owner of the IPC capability.
 * @param i The index of the IPC capability.
 * @param data The data to send in the reply.
 * @param capty The type of capability to send.
 * @param j The index of the capability to send.
 * @param next Pointer to store the next process to run.
 * @return ERR_SUCCESS on success, or an error code on failure.
 */
int ipc_reply(pid_t owner, index_t i, word_t data[2], capty_t capty, index_t j, proc_t **next);

/**
 * @brief Replies to a received message and immediately receives another message.
 * @param owner The owner of the IPC capability.
 * @param i The index of the IPC capability.
 * @param data The data to send in the reply.
 * @param capty The type of capability to send.
 * @param j The index of the capability to send.
 * @param next Pointer to store the next process to run.
 * @return ERR_SUCCESS on success, or an error code on failure.
 */
int ipc_replyrecv(pid_t owner, index_t i, word_t data[2], capty_t capty, index_t j, proc_t **next);

/**
 * @brief Asynchronously sends data to another process.
 *
 * This function allows non-blocking communication between processes.
 *
 * @param owner The owner of the IPC capability.
 * @param i The index of the IPC capability.
 * @param data The data to send.
 * @param next Pointer to store the next process to run.
 * @return ERR_SUCCESS on success, or an error code on failure.
 */
int ipc_asend(pid_t owner, index_t i, word_t data, proc_t **next);

/**
 * @brief Asynchronously receives data from another process.
 * @param owner The owner of the IPC capability.
 * @param i The index of the IPC capability.
 * @param data Pointer to store the received data.
 * @return ERR_SUCCESS on success, or an error code on failure.
 */
int ipc_arecv(pid_t owner, index_t i, word_t *data);
