# S3K API Reference

This document describes the S3K system call API as provided by the S3K library. Each function is available to user processes and corresponds to a kernel handler.

---

## Process and Synchronization

- `s3k_pid_t s3k_pid_get(void)`
	- Returns the current process's unique identifier (PID).

- `s3k_word_t s3k_vreg_get(s3k_vreg_t reg)`
	- Returns the value of the specified virtual register.

- `void s3k_vreg_set(s3k_vreg_t reg, s3k_word_t val)`
	- Sets the value of the specified virtual register.

- `void s3k_sync(void)`
	- Releases the process and yields to the scheduler. Use to voluntarily yield CPU time.

- `void s3k_sleep_until(s3k_time_t time)`
	- Puts the process to sleep until the specified absolute time (in system ticks).

---

## Capability Management

### Memory Capabilities

- `int s3k_mem_get(s3k_index_t i, s3k_cap_mem_t *cap)`
	- Retrieve a memory capability at index `i` into `cap`.
- `int s3k_mem_derive(s3k_index_t i, s3k_fuel_t cfree, s3k_mem_perm_t perm, s3k_mem_addr_t begin, s3k_mem_addr_t end)`
	- Derive a new memory capability from index `i` with the given permissions and address range.
- `int s3k_mem_revoke(s3k_index_t i)`
	- Revoke all children derived from the memory capability at index `i`.
- `int s3k_mem_delete(s3k_index_t i)`
	- Delete the memory capability at index `i`.
- `int s3k_mem_pmp_get(s3k_index_t i, s3k_pmp_slot_t *slot, s3k_mem_perm_t *perm, s3k_pmp_addr_t *addr)`
	- Get the PMP (Physical Memory Protection) configuration for the memory capability at index `i`.
- `int s3k_mem_pmp_set(s3k_index_t i, s3k_pmp_slot_t slot, s3k_mem_perm_t perm, s3k_pmp_addr_t addr)`
	- Set the PMP configuration for the memory capability at index `i`.
- `int s3k_mem_pmp_clear(s3k_index_t i)`
	- Clear the PMP configuration for the memory capability at index `i`.

### Time Slice Capabilities

- `int s3k_tsl_get(s3k_index_t i, s3k_cap_tsl_t *cap)`
	- Retrieve a time slice capability at index `i` into `cap`.
- `int s3k_tsl_derive(s3k_index_t i, s3k_fuel_t cfree, bool enabled, s3k_time_slot_t length)`
	- Derive a new time slice capability from index `i`.
- `int s3k_tsl_revoke(s3k_index_t i)`
	- Revoke all children derived from the time slice capability at index `i`.
- `int s3k_tsl_delete(s3k_index_t i)`
	- Delete the time slice capability at index `i`.
- `int s3k_tsl_set(s3k_index_t i, bool enabled)`
	- Enable or disable the time slice capability at index `i`.

### Monitor Capabilities

- `int s3k_mon_get(s3k_index_t i, s3k_cap_mon_t *cap)`
	- Retrieve a monitor capability at index `i` into `cap`.
- `int s3k_mon_derive(s3k_index_t i, s3k_fuel_t cfree)`
	- Derive a new monitor capability from index `i`.
- `int s3k_mon_revoke(s3k_index_t i)`
	- Revoke all children derived from the monitor capability at index `i`.
- `int s3k_mon_delete(s3k_index_t i)`
	- Delete the monitor capability at index `i`.
- `int s3k_mon_suspend(s3k_index_t i)`
	- Suspend the process monitored by the monitor capability at index `i`.
- `int s3k_mon_resume(s3k_index_t i)`
	- Resume the process monitored by the monitor capability at index `i`.
- `int s3k_mon_yield(s3k_index_t i)`
	- Yield execution time to the process monitored by the monitor capability at index `i`.
- `int s3k_mon_reg_set(s3k_index_t i, s3k_reg_t reg, s3k_word_t val)`
	- Set a register value for the process monitored by the monitor capability at index `i`.
- `int s3k_mon_reg_get(s3k_index_t i, s3k_reg_t reg, s3k_word_t *val)`
	- Get a register value for the process monitored by the monitor capability at index `i`.
- `int s3k_mon_vreg_set(s3k_index_t i, s3k_vreg_t reg, s3k_word_t val)`
	- Set a virtual register value for the process monitored by the monitor capability at index `i`.
- `int s3k_mon_vreg_get(s3k_index_t i, s3k_vreg_t reg, s3k_word_t *val)`
	- Get a virtual register value for the process monitored by the monitor capability at index `i.

### IPC Capabilities

- `int s3k_ipc_get(s3k_index_t i, s3k_cap_ipc_t *cap)`
	- Retrieve an IPC capability at index `i` into `cap`.
- `int s3k_ipc_derive(s3k_index_t i, s3k_fuel_t cfree, s3k_ipc_mode_t mode, s3k_ipc_flag_t flag)`
	- Derive a new IPC capability from index `i`.
- `int s3k_ipc_revoke(s3k_index_t i)`
	- Revoke all children derived from the IPC capability at index `i`.
- `int s3k_ipc_delete(s3k_index_t i)`
	- Delete the IPC capability at index `i`.

---

## Monitor Operations

These functions allow a monitor to operate on another process's capabilities. The first index `i` is the monitor capability, and the second index `j` is the target process's capability.

- `int s3k_mon_mem_get(s3k_index_t i, s3k_index_t j, s3k_cap_mem_t *cap)`
	- Get a memory capability from another process.
- `int s3k_mon_tsl_get(s3k_index_t i, s3k_index_t j, s3k_cap_tsl_t *cap)`
	- Get a time slice capability from another process.
- `int s3k_mon_mon_get(s3k_index_t i, s3k_index_t j, s3k_cap_mon_t *cap)`
	- Get a monitor capability from another process.
- `int s3k_mon_ipc_get(s3k_index_t i, s3k_index_t j, s3k_cap_ipc_t *cap)`
	- Get an IPC capability from another process.
- `int s3k_mon_mem_grant(s3k_index_t i, s3k_index_t j)`
	- Grant a memory capability to another process.
- `int s3k_mon_tsl_grant(s3k_index_t i, s3k_index_t j)`
	- Grant a time slice capability to another process.
- `int s3k_mon_mon_grant(s3k_index_t i, s3k_index_t j)`
	- Grant a monitor capability to another process.
- `int s3k_mon_ipc_grant(s3k_index_t i, s3k_index_t j)`
	- Grant an IPC capability to another process.
- `int s3k_mon_mem_derive(s3k_index_t i, s3k_index_t j, s3k_fuel_t cfree, s3k_mem_perm_t perm, s3k_mem_addr_t begin, s3k_mem_addr_t end)`
	- Derive and grant a memory capability to another process.
- `int s3k_mon_tsl_derive(s3k_index_t i, s3k_index_t j, s3k_fuel_t cfree, bool enabled, s3k_time_slot_t length)`
	- Derive and grant a time slice capability to another process.
- `int s3k_mon_mon_derive(s3k_index_t i, s3k_index_t j, s3k_fuel_t cfree)`
	- Derive and grant a monitor capability to another process.
- `int s3k_mon_ipc_derive(s3k_index_t i, s3k_index_t j, s3k_fuel_t cfree, s3k_ipc_mode_t mode, s3k_ipc_flag_t flag)`
	- Derive and grant an IPC capability to another process.
- `int s3k_mon_mem_pmp_get(s3k_index_t i, s3k_index_t j, s3k_pmp_slot_t *slot, s3k_mem_perm_t *perm, s3k_pmp_addr_t *addr)`
	- Get PMP configuration for a memory capability in another process.
- `int s3k_mon_mem_pmp_set(s3k_index_t i, s3k_index_t j, s3k_pmp_slot_t slot, s3k_mem_perm_t perm, s3k_pmp_addr_t addr)`
	- Set PMP configuration for a memory capability in another process.
- `int s3k_mon_mem_pmp_clear(s3k_index_t i, s3k_index_t j)`
	- Clear PMP configuration for a memory capability in another process.
- `int s3k_mon_tsl_set(s3k_index_t i, s3k_index_t j, bool enabled)`
	- Enable or disable a time slice capability in another process.

---

## IPC Operations

These functions implement inter-process communication (IPC) using capabilities.

- `int s3k_ipc_send(s3k_index_t i, s3k_word_t msg[2], s3k_capty_t capty, s3k_index_t j)`
	- Send a synchronous IPC message (optionally transferring a capability) to another process.
- `int s3k_ipc_recv(s3k_index_t i, s3k_word_t msg[2], s3k_capty_t *capty, s3k_index_t *j, uint32_t servtime)`
	- Wait to receive a synchronous IPC message.
- `int s3k_ipc_call(s3k_index_t i, s3k_word_t msg[2], s3k_capty_t *capty, s3k_index_t *j)`
	- Make a synchronous IPC call and wait for a reply.
- `int s3k_ipc_reply(s3k_index_t i, s3k_word_t msg[2], s3k_capty_t capty, s3k_index_t j)`
	- Send a reply to a synchronous IPC call.
- `int s3k_ipc_replyrecv(s3k_index_t i, s3k_word_t msg[2], s3k_capty_t *capty, s3k_index_t *j, uint32_t servtime)`
	- Send a reply and then wait to receive a new IPC message (atomic operation).
- `int s3k_ipc_asend(s3k_index_t i, s3k_word_t msg)`
	- Send an asynchronous IPC message.
- `int s3k_ipc_arecv(s3k_index_t i, s3k_word_t *msg)`
	- Receive an asynchronous IPC message.

---

## Utility Functions

See `s3k/util.h` for address encoding/decoding helpers:

- `s3k_word_t s3k_pmp_napot_encode(s3k_word_t base, s3k_word_t size)`
	- Encode a base address and size using RISC-V NAPOT format for PMP.
- `s3k_word_t s3k_pmp_napot_decode_base(s3k_word_t addr)`
	- Decode the base address from a NAPOT-encoded PMP address.
- `s3k_word_t s3k_pmp_napot_decode_size(uint64_t addr)`
	- Decode the size from a NAPOT-encoded PMP address.
