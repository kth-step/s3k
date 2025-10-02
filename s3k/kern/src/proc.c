#include "proc.h"

#include "csr.h"
#include "types.h"

/**
 * Table of processes.
 */
static proc_t procs[MAX_PID];

/**
 * Retrieves the process control block (PCB) for a given process ID (PID).
 */
static inline proc_t *_proc(pid_t pid)
{
	return &procs[pid - 1]; // Retrieve the process control block.
}

/**
 * Initializes the process table with default values.
 */
void proc_init(word_t init)
{
	for (pid_t i = 1; i <= MAX_PID; i++) {
		_proc(i)->state = PROC_STATE_SUSPENDED; // Initialize all processes to the suspended state.
		_proc(i)->regs.pc = 0;			// Set the program counter to 0 for all processes.
		_proc(i)->pid = i;
	}
	_proc(1)->state = PROC_STATE_READY; // Set the first process to the ready state.
	_proc(1)->regs.pc = init;	    // Set the initial program counter for the first process.
}

/**
 * Retrieves a process by its PID.
 */
proc_t *proc_get(pid_t pid)
{
	return _proc(pid); // Retrieve the process control block.
}

/**
 * Retrieves the PID of a process.
 */
pid_t proc_get_pid(const proc_t *p)
{
	return p->pid; // Calculate the PID based on the pointer offset.
}

/**
 * Sets a PMP slot for a process.
 */
void proc_pmp_set(pid_t pid, pmp_slot_t slot, mem_perm_t rwx, pmp_addr_t addr)
{
	_proc(pid)->pmp.cfg[slot] = PMP_MODE_NAPOT | rwx; // Set PMP permissions.
	_proc(pid)->pmp.addr[slot] = addr;		  // Set PMP address.
}

/**
 * Clears a PMP slot for a process.
 */
void proc_pmp_clear(pid_t pid, pmp_slot_t slot)
{
	_proc(pid)->pmp.cfg[slot] = 0;	// Clear PMP permissions.
	_proc(pid)->pmp.addr[slot] = 0; // Clear PMP address.
}

/**
 * Checks if a PMP slot is set for a process.
 */
bool proc_pmp_is_set(pid_t pid, pmp_slot_t slot)
{
	return _proc(pid)->pmp.addr[slot] != 0; // Check if the PMP slot is set.
}

/**
 * Retrieves the PMP configuration for a process.
 */
void proc_pmp_get(pid_t pid, pmp_slot_t slot, mem_perm_t *rwx, pmp_addr_t *addr)
{
	*rwx = _proc(pid)->pmp.cfg[slot] & MEM_PERM_RWX; // Get PMP permissions.
	*addr = _proc(pid)->pmp.addr[slot];		 // Get PMP address.
}

/**
 * Sets a register value for a process.
 */
void proc_set_register(pid_t pid, word_t regid, word_t value)
{
	word_t *reg = (word_t *)&(_proc(pid)->regs);
	reg[regid] = value; // Set the register value.
}

/**
 * Gets a register value for a process.
 */
word_t proc_get_register(pid_t pid, word_t regid)
{
	word_t *reg = (word_t *)&(_proc(pid)->regs);
	return reg[regid]; // Get the register value.
}

/**
 * Acquires a process by its PID.
 */
bool proc_acquire(pid_t pid)
{
	proc_state_t state = _proc(pid)->state;
	if (state != PROC_STATE_READY) {
		return false;
	}
	_proc(pid)->state = PROC_STATE_ACQUIRED; // Mark the process as acquired.
	return true;
}

/**
 * Suspends a process by its PID.
 */
void proc_suspend(pid_t pid)
{
	_proc(pid)->state &= PROC_STATE_ACQUIRED;  // Remove all other states but acquired.
	_proc(pid)->state |= PROC_STATE_SUSPENDED; // Mark the process as suspended.
}

/**
 * Resumes a suspended process.
 */
void proc_resume(pid_t pid)
{
	_proc(pid)->state &= ~PROC_STATE_SUSPENDED; // Remove the suspended state.
}

/**
 * Acquires a process by its PID for IPC.
 * The index is used to identify the IPC capability used when calling proc_ipc_block.
 */
bool proc_ipc_acquire(pid_t pid, index_t i)
{
	word_t expected = PROC_STATE_BLOCKED | i << 4;
	word_t desired = PROC_STATE_ACQUIRED;

	if (_proc(pid)->state != expected) {
		return false; // Process is not in the expected state, cannot acquire.
	}
	_proc(pid)->state = desired; // Mark the process as acquired.
	return true;
}

/**
 * Blocks a process by its PID for IPC.
 * The index is used to identify the IPC capability used.
 */
bool proc_ipc_block(pid_t pid, index_t i)
{
	word_t expected = PROC_STATE_ACQUIRED;
	word_t desired = PROC_STATE_BLOCKED | PROC_STATE_ACQUIRED | i << 4;
	if (_proc(pid)->state != expected) {
		return false; // Process is not in the expected state, cannot block.
	}
	_proc(pid)->state = desired; // Mark the process as blocked.
	return true;
}

/**
 * Releases a process by its PID.
 */
void proc_release(pid_t pid)
{
	_proc(pid)->state &= ~PROC_STATE_ACQUIRED; // Mark the process as ready.
}
