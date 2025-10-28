#include "ipc.h"

#include "macro.h"
#include "mem.h"
#include "mon.h"
#include "preempt.h"
#include "tsl.h"

/**
 * Table of IPC capabilities.
 */
static ipc_t ipc_table[IPC_TABLE_SIZE];

/**
 * Initialize the IPC capabilities.
 */
void ipc_init(void)
{
	ipc_table[0] = (ipc_t){
		.owner = 1,
		.cfree = MAX_IPC_FUEL,
		.csize = MAX_IPC_FUEL,
	};
}

/**
 * Check if the IPC capability is valid for the given owner and index.
 */
bool ipc_valid_access(pid_t owner, index_t i)
{
	return i < ARRAY_SIZE(ipc_table) && ipc_table[i].owner == owner;
}

/**
 * Check if the IPC capability can be derived.
 */
static bool _valid_derivation(ipc_t *cap, fuel_t csize, ipc_mode_t mode, ipc_flag_t flag)
{
	// Check if sufficient cfree is available.
	if (csize >= cap->cfree && csize > 0) {
		return false;
	}

	if ((cap->mode & IPC_MODE_REVOKE) != 0) {
		// Cannot derive if revoke flag is set.
		return false;
	}

	if (cap->mode == IPC_MODE_NONE) {
		// If deriving a sink IPC capability or NULL IPC capability.
		return mode != IPC_MODE_NONE || flag == 0;
	}
	// If deriving a source IPC capability
	return (cap->mode == mode && cap->flag == flag && csize == 1);
}

/**
 * Check if flags and access rights permit sending a specific capability.
 */
static bool _valid_capability_send(pid_t owner, index_t i, capty_t capty, ipc_flag_t flag)
{
	switch (capty) {
	case CAPTY_NONE:
		return true;
	case CAPTY_MEM:
		return (flag & IPC_FLAG_MEM) && mem_valid_access(owner, i);
	case CAPTY_TSL:
		return (flag & IPC_FLAG_TSL) && tsl_valid_access(owner, i);
	case CAPTY_MON:
		return (flag & IPC_FLAG_MON) && mon_valid_access(owner, i);
	case CAPTY_IPC:
		return (flag & IPC_FLAG_IPC) && ipc_valid_access(owner, i);
	default:
		return false;
	}
}

/**
 * Transfer an IPC capability from one process to another.
 */
int ipc_transfer(pid_t owner, index_t i, pid_t new_owner)
{
	if (UNLIKELY(!ipc_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}
	ipc_table[i].owner = new_owner;
	return ERR_SUCCESS;
}

/**
 * Get an IPC capability for the given owner and index.
 */
int ipc_get(pid_t owner, index_t i, ipc_t *cap)
{
	if (UNLIKELY(!ipc_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}
	*cap = ipc_table[i];
	return ERR_SUCCESS;
}

/**
 * Derive a new IPC capability.
 */
int ipc_derive(pid_t owner, index_t i, pid_t target, fuel_t csize, ipc_mode_t mode, ipc_flag_t flag)
{
	if (UNLIKELY(!ipc_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	// Apply masks to mode and flag.
	mode &= IPC_MODE_MASK;
	flag &= IPC_FLAG_MASK;

	// Check if the derivation is valid.
	if (UNLIKELY(!_valid_derivation(&ipc_table[i], csize, mode, flag))) {
		return ERR_INVALID_ARGUMENT;
	}

	// Subtract delegated capability size from cfree.
	ipc_table[i].cfree -= csize;

	// Calculate the new index for the derived capability.
	index_t j = i + ipc_table[i].cfree;

	// Add the new IPC capability to the table.
	ipc_table[j] = (ipc_t){
		.owner = target,
		.cfree = csize,
		.csize = csize,
		.mode = mode,
		.flag = flag,
		.sink = (ipc_table[i].mode == IPC_MODE_NONE) ? j : ipc_table[i].sink,
		.source = 0,
	};

	// Return the index of the new capability.
	return j;
}

/**
 * Revoke an IPC capability.
 */
int ipc_revoke(pid_t owner, index_t i)
{
	if (UNLIKELY(!ipc_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	// Only non-endpoint IPC capabilities can revoke children.
	if ((ipc_table[i].mode & IPC_MODE_MASK) != IPC_MODE_NONE) {
		return ERR_INVALID_ARGUMENT;
	}

	// Revoke children of IPC capability.
	while (ipc_table[i].cfree < ipc_table[i].csize) {
		// Get the next child index.
		index_t j = i + ipc_table[i].cfree;

		// Invalidate the child capability.
		ipc_table[j].owner = INVALID_PID;

		// Reclaim the child's capability table.
		ipc_table[i].cfree += ipc_table[j].cfree;

		// Set revoke flag to prevent further derivations.
		ipc_table[i].mode = IPC_MODE_REVOKE;

		// Check for preemption.
		if (UNLIKELY(preempt()))
			break;
	}

	if (ipc_table[i].cfree == ipc_table[i].csize) {
		// Unset revoke flag if all children are revoked.
		ipc_table[i].mode = IPC_MODE_NONE;
	}

	// Return number of unrevoked capability slots.
	return ipc_table[i].csize - ipc_table[i].cfree;
}

/**
 * Delete an IPC capability.
 */
int ipc_delete(pid_t owner, index_t i)
{
	if (UNLIKELY(!ipc_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	// Invalidate the capability.
	ipc_table[i].owner = INVALID_PID;

	return ERR_SUCCESS;
}

/**
 * Send data and potentially a capability to the receiver.
 * For synchronous IPC only!
 */
static void do_send(pid_t receiver, word_t data[2], pid_t owner, capty_t capty, index_t i)
{
	// Send the data to the target process.
	proc_t *proc = proc_get(receiver);
	// Receiver has successfully received the data.
	proc->regs.a0 = ERR_SUCCESS;
	// Copy data.
	proc->regs.a1 = data[0];
	proc->regs.a2 = data[1];
	// Copy capability information.
	proc->regs.a3 = capty;
	proc->regs.a4 = (capty == CAPTY_NONE) ? 0 : i;
	switch (capty) {
	case CAPTY_NONE:
		break;
	case CAPTY_MEM:
		mem_transfer(owner, i, receiver);
		break;
	case CAPTY_TSL:
		tsl_transfer(owner, i, receiver);
		break;
	case CAPTY_MON:
		mon_transfer(owner, i, receiver);
		break;
	case CAPTY_IPC:
		ipc_transfer(owner, i, receiver);
		break;
	default:
		__builtin_unreachable();
	}
}

/**
 * Check if the IPC invocation has valid access.
 */
static bool _ipc_invoke_valid_access(pid_t owner, index_t i, ipc_mode_t mode, bool sink)
{
	if (UNLIKELY(!ipc_valid_access(owner, i))) {
		return false;
	}
	// Check if the IPC capability is in the correct mode.
	if (ipc_table[i].mode != mode) {
		return false;
	}
	// If sink, check if it is a sink capability.
	// Otherwise, check if it is a source capability.
	return sink ? (ipc_table[i].sink == i) : (ipc_table[i].sink != i);
}

/**
 * Send data and potentially a capability to the receiver.
 * For synchronous unidirectional IPC only!
 */
int ipc_send(pid_t owner, index_t i, word_t data[2], capty_t capty, index_t j, proc_t **next)
{
	if (UNLIKELY(!_ipc_invoke_valid_access(owner, i, IPC_MODE_USYNC, false))) {
		return ERR_INVALID_ACCESS;
	}
	if (UNLIKELY(!_valid_capability_send(owner, j, capty, ipc_table[i].flag))) {
		return ERR_INVALID_ARGUMENT;
	}

	// Get the sink capability.
	index_t sink = ipc_table[i].sink;

	// Get the receiver process.
	pid_t receiver = ipc_table[sink].owner;

	// Check if the receiver is ready.
	if (receiver == INVALID_PID || !proc_ipc_acquire(receiver, sink)) {
		return ERR_INVALID_STATE;
	}

	// Send the data to the target process.
	do_send(receiver, data, owner, capty, j);

	proc_t *sender = *next;
	if (ipc_table[i].flag & IPC_FLAG_YIELD) {
		// If yielding IPC, set the next process to the receiver.
		*next = proc_get(receiver);
		// Receiver inherits the sender's timeout.
		(*next)->timeout = sender->timeout;
	} else {
		// If not yielding IPC, release the receiver.
		proc_release(receiver);
	}
	return ERR_SUCCESS;
}

/**
 * Receive data and potentially a capability from the sender.
 * For synchronous IPC only!
 */
int ipc_recv(pid_t owner, index_t i, proc_t **next)
{
	if (!_ipc_invoke_valid_access(owner, i, IPC_MODE_USYNC, true)
	    && _ipc_invoke_valid_access(owner, i, IPC_MODE_BSYNC, true)) {
		return ERR_INVALID_ACCESS;
	}
	// Go to a receiver state.
	proc_ipc_block(owner, i);
	(*next)->timeout = UINT64_MAX;
	*next = NULL;
	return ERR_SUCCESS;
}

/**
 * Send a synchronous IPC call to the receiver.
 */
int ipc_call(pid_t owner, index_t i, word_t data[2], capty_t capty, index_t j, proc_t **next)
{
	if (!_ipc_invoke_valid_access(owner, i, IPC_MODE_BSYNC, false)) {
		return ERR_INVALID_ACCESS;
	}
	if (!_valid_capability_send(owner, j, capty, ipc_table[i].flag)) {
		return ERR_INVALID_ARGUMENT;
	}
	// Get the sink capability and receiver process.
	index_t sink = ipc_table[i].sink;
	pid_t receiver = ipc_table[sink].owner;

	// If receiver is invalid or not ready, return invalid state error.
	if (receiver == INVALID_PID || !proc_ipc_acquire(receiver, sink)) {
		return ERR_INVALID_STATE;
	}

	// Perform the send operation.
	do_send(receiver, data, owner, capty, j);

	// Set the receiver's source capability.
	ipc_table[sink].source = i;

	// Wait for reply.
	proc_ipc_block(owner, i);

	proc_t *sender = *next;
	if (ipc_table[i].flag & IPC_FLAG_YIELD) {
		// If yielding IPC, set the next process to the receiver.
		*next = proc_get(receiver);
		// Receiver inherits the sender's timeout.
		(*next)->timeout = sender->timeout;
	} else {
		// Release the receiver.
		proc_release(receiver);
		sender->timeout = UINT64_MAX;
	}
	return ERR_TIMEOUT;
}

/**
 * Reply to a synchronous IPC call.
 */
int ipc_reply(pid_t owner, index_t i, word_t data[2], capty_t capty, index_t j, proc_t **next)
{
	if (!_ipc_invoke_valid_access(owner, i, IPC_MODE_BSYNC, true)) {
		return ERR_INVALID_ACCESS;
	}
	if (!_valid_capability_send(owner, j, capty, ipc_table[i].flag)) {
		return ERR_INVALID_ARGUMENT;
	}
	// Get the source capability and client process.
	index_t source = ipc_table[i].source;
	pid_t receiver_pid = ipc_table[source].owner;

	// Check if the client is valid and ready.
	if (receiver_pid == INVALID_PID || !proc_ipc_acquire(receiver_pid, source)) {
		return ERR_INVALID_STATE;
	}

	// Send the operation.
	do_send(receiver_pid, data, owner, capty, j);
	ipc_table[i].source = 0; // Clear the source capability.

	proc_t *sender = *next;
	proc_t *receiver = proc_get(receiver_pid);

	if (ipc_table[i].flag & IPC_FLAG_YIELD) {
		// If yielding IPC, set the next process to the receiver.
		*next = receiver;
		// The receiver inherits the timeout.
		receiver->timeout = sender->timeout;
	} else {
		// If not yielding IPC, release the receiver.
		// Set timeout to 0 so it can be scheduled as soon as possible.
		proc_release(receiver_pid);
		receiver->timeout = 0;
	}
	return ERR_SUCCESS;
}

/**
 * Reply and receive in a single IPC operation.
 */
int ipc_replyrecv(pid_t owner, index_t i, word_t data[2], capty_t capty, index_t j, proc_t **next)
{
	if (!_ipc_invoke_valid_access(owner, i, IPC_MODE_BSYNC, true)) {
		return ERR_INVALID_ACCESS;
	}

	if (!_valid_capability_send(owner, j, capty, ipc_table[i].flag)) {
		return ERR_INVALID_ARGUMENT;
	}

	proc_t *sender = *next;

	// Set next to null by default.
	*next = NULL;

	// Get the source and sink capabilities.
	index_t source = ipc_table[i].source;
	pid_t recv_pid = ipc_table[source].owner;

	if (recv_pid != INVALID_PID && proc_ipc_acquire(recv_pid, source)) {
		// Do send operation.
		do_send(recv_pid, data, owner, capty, j);

		proc_t *receiver = proc_get(recv_pid);

		if (ipc_table[i].flag & IPC_FLAG_YIELD) {
			// If yielding IPC, set the next process to the receiver.
			*next = receiver;
			// Receiver inherits the sender's timeout.
			receiver->timeout = sender->timeout;
		} else {
			// If not yielding IPC, release the receiver.
			proc_release(recv_pid);
			// Set timeout to 0 so it can be scheduled as soon as possible.
			receiver->timeout = 0;
		}
	}

	// Perform receive operation.
	proc_ipc_block(owner, i);
	sender->timeout = UINT64_MAX;

	return ERR_SUCCESS;
}

/**
 * Asynchronously send data.
 */
int ipc_asend(pid_t owner, index_t i, word_t data, proc_t **next)
{
	if (!_ipc_invoke_valid_access(owner, i, IPC_MODE_ASYNC, false)) {
		return ERR_INVALID_ACCESS;
	}

	index_t sink = ipc_table[i].sink;
	pid_t recv_pid = ipc_table[sink].owner;

	// Data stored in the source field.
	ipc_table[sink].source = data;

	if ((ipc_table[i].flag & IPC_FLAG_YIELD) && recv_pid != INVALID_PID && proc_acquire(recv_pid)) {
		proc_t *sender = *next;
		proc_t *receiver = proc_get(recv_pid);
		*next = receiver;
		receiver->timeout = sender->timeout;
	}
	return ERR_SUCCESS;
}

/**
 * Asynchronously receive data.
 */
int ipc_arecv(pid_t owner, index_t i, word_t *data)
{
	if (!_ipc_invoke_valid_access(owner, i, IPC_MODE_ASYNC, true)) {
		return ERR_INVALID_ACCESS;
	}
	// Read data from the source field.
	*data = ipc_table[i].source;
	return ERR_SUCCESS;
}
