# S3K API
---
## System calls
- Get local information
  - [s3k_get_pid](#s3k_get_pid) - Get the process ID.
  - [s3k_get_hartid](#s3k_get_hartid) - Get the hardware thread ID.
  - [s3k_get_time](#s3k_get_time) - Get the current real-time.
  - [s3k_get_timeout](#s3k_get_timeout) - Get the timeout of the current minor frame.
- Write and read registers
  - [s3k_reg_read](#s3k_reg_read) - Read from register.
  - [s3k_reg_write](#s3k_reg_write) - Write to register.
- Process synchronization
  - [s3k_sync](#s3k_sync) - Synchronize process's memory and time allocation with capabilities.
  - [s3k_sync_mem](#s3k_sync_mem) - Synchronize process's memory with capabilities.
- Common capability operations
  - [s3k_cap_read](#s3k_cap_read) - Read capability's descriptor.
  - [s3k_cap_move](#s3k_cap_move) - Move a capability.
  - [s3k_cap_delete](#s3k_cap_delete) - Delete a capability.
  - [s3k_cap_revoke](#s3k_cap_revoke) - Revoke child capabilities.
  - [s3k_cap_derive](#s3k_cap_derive) - Derive a new capability.
- PMP capability invocations
  - [s3k_pmp_load](#s3k_pmp_load) - Load PMP capability into PMP slot.
  - [s3k_pmp_unload](#s3k_pmp_load) - Unload a PMP capability.
- Monitor capability invocations
  - [s3k_monitor_suspend](#s3k_monitor_suspend) - Suspend a process.
  - [s3k_monitor_resume](#s3k_monitor_resume) - Resume a process.
  - [s3k_monitor_reg_read](#s3k_monitor_reg_read) - Read from process's register.
  - [s3k_monitor_reg_write](#s3k_monitor_reg_write) - Write to process's register.
  - [s3k_monitor_pmp_load](#s3k_monitor_pmp_load) - Load a PMP capability into PMP slot.
  - [s3k_monitor_pmp_unload](#s3k_monitor_pmp_unload) - Unload a PMP capability.
  - [s3k_monitor_cap_move](#s3k_monitor_cap_move) - Move a capability.
- IPC capability invocations
  - [s3k_sock_send](#s3k_sock_send) - Send a packet to the server.
  - [s3k_sock_call](#s3k_sock_call) - Make an RPC.
  - [s3k_sock_reply](#s3k_sock_reply) - Send a packet to the client.
  - [s3k_sock_recv](#s3k_sock_recv) - Receive a packet from client.
  - [s3k_sock_replyrecv](#s3k_sock_replyrecv) - Send then receive a packet from client.

### s3k_get_pid
```c
int s3k_get_pid(void);
```  
Returns the process ID of the caller.

**Return:** the process ID of the caller

### s3k_get_time
```c
uint64_t s3k_get_time(void);
```
Returns the current real-time.

**Return:** the current real-time.


### s3k_get_timeout
```c
uint64_t s3k_get_timeout(void);
```
Returns the timeout of the current minor frame.

**Return:** the timeout of the current minor frame.


### s3k_reg_read
```c
uint64_t s3k_reg_read(uint64_t reg);
```
Returns the value of register.

**Parameters:**
- `reg` - index of register.

**Return:** the value of register with `reg`. If `reg` is invalid, returns 0.


### s3k_reg_write
```c
void s3k_reg_write(uint64_t reg, uint64_t val);
```
Sets the value of a register. Has no effect if `reg` is invalid.

**Parameters:**
- `reg` - ID of register.
- `val` - value to write to the register.

**Note:** Setting the standard RISC-V registers may have unintended consequences.


### s3k_sync
```c
void s3k_sync(void);
```
Synchronize the process's memory and time permissions with the underlying capabilities.


### s3k_sync_mem
```c
void s3k_sync_mem(void);
```
Synchronize the process's memory permissions with the underlying capabilities.


### s3k_cap_read
```c
error_t s3k_cap_read(uint64_t i, cap_t *cap);
```
Read the descriptor of the i'th capability.

**Parameters:**
- `i` - index of capability.
- `cap` - buffer to write the descriptor.

**Returns:**
- `S3K_SUCCESS` - if descriptor was read.
- `S3K_ERR_INVALID_INDEX` - if `i` is invalid (`i >= N_CAP`).

### s3k_cap_move
```c
error_t s3k_cap_move(uint64_t i, uint64_t j);
```
Move the i'th capability to the j'th slot.

**Parameters:**
- `i` - source index of capability to move.
- `j` - destination index of capability to move.

**Returns:**
- `S3K_SUCCESS` - if capability was moved.
- `S3K_PREEMPTED` - if system call was aborted due to timer preemption.
- `S3K_ERR_INVALID_INDEX` - if `i` is invalid (`i >= N_CAP`).
- `S3K_ERR_INVALID_INDEX` - if `j` is invalid (`j >= N_CAP`).
- `S3K_ERR_SRC_EMPTY` - if the i'th slot was empty.
- `S3K_ERR_DST_OCCUPIED` - if the j'th slot was not empty


### s3k_cap_delete
```c
error_t s3k_cap_delete(uint64_t i);
```
Delete the i'th capability.

**Parameters:**
- `i` - index of capability to delete.

**Returns:**
- `S3K_SUCCESS` - if capability was deleted.
- `S3K_PREEMPTED` - if system call was aborted due to timer preemption.
- `S3K_ERR_INVALID_INDEX` - if `i` is invalid (`i >= N_CAP`).
- `S3K_ERR_EMPTY` - if the i'th slot was empty.

### s3k_cap_revoke
```c
error_t s3k_cap_revoke(uint64_t i);
```
Recursively deletes the children of the i'th capability, then restore the i'th capability to its original state.

**Parameters:**
- `i` - index of capability to revoke with.

**Returns:**
- `S3K_SUCCESS` - if capabilities were revoked and restored.
- `S3K_PREEMPTED` - if system call was aborted due to timer preemption.
- `S3K_ERR_INVALID_INDEX` - if `i` is invalid (`i >= N_CAP`).
- `S3K_ERR_EMPTY` - if the i'th slot was empty.


### s3k_cap_derive
```c
error_t s3k_cap_derive(uint64_t i, uint64_t j, cap_t cap);
```
Create a new capability at the j'th slot using the i'th capability. 
If successful, the i'th capability is updated to reflect that resources were reallocated (only slice capabilities).

**Parameters:**
- `i` - index of capability to derive from.
- `j` - destination of the new capability.
- `cap` - description of capability to create

**Returns:**
- `S3K_SUCCESS` - if the new capability wass successfully created.
- `S3K_PREEMPTED` - if system call was aborted due to timer preemption.
- `S3K_ERR_INVALID_INDEX` - if `i` is invalid (`i >= N_CAP`).
- `S3K_ERR_INVALID_INDEX` - if `j` is invalid (`j >= N_CAP`).
- `S3K_ERR_EMPTY` - if the i'th slot was empty.
- `S3K_ERR_DST_OCCUPIED` - if the j'th slot was occupied.
- `S3K_ERR_INVALID_CAPABILITY` - if i'th capability can not be used to derive `cap`

**Note:** If preempted, the syscall may be partially; that is, only some child capabilities have been revoked.

### s3k_pmp_load
```c
error_t s3k_pmp_load(uint64_t i, uint64_t j)
```
Use the i'th capability to set the j'th PMP slot. 

The i'th capability is valid if it is a PMP capability not in use.

**Parameters:**
- `i` - source index of PMP capability to load
- `j` - index of PMP slot

**Returns:**
- `S3K_SUCCESS` - if PMP capability was loaded.
- `S3K_PREEMPTED` - if system call was aborted due to timer preemption.
- `S3K_ERR_INVALID_INDEX` - if `i` is invalid (`i >= N_CAP`).
- `S3K_ERR_INVALID_SLOT` - if `j` is invalid (`j >= N_PMP`).
- `S3K_ERR_EMPTY` - if the i'th capability slot was empty.
- `S3K_ERR_DST_OCCUPIED` - if the j'th PMP slot was occupied.
- `S3K_ERR_INVALID_PMP` - if i'th capability is invalid.

### s3k_pmp_unload
```c
error_t s3k_pmp_unload(uint64_t i)
```
Unload the i'th capability. 

The i'th capability is valid if it is a PMP capability in use.

**Parameters:**
- `i` - source index of PMP capability to load
- `j` - index of PMP slot

**Returns:**
- `S3K_SUCCESS` - if PMP capability was unloaded.
- `S3K_PREEMPTED` - if system call was aborted due to timer preemption.
- `S3K_ERR_INVALID_INDEX` - if `i` is invalid (`i >= N_CAP`).
- `S3K_ERR_EMPTY` - if the i'th capability slot was empty.
- `S3K_ERR_INVALID_PMP` - if i'th capability is invalid.

### s3k_monitor_suspend
```c
error_t s3k_monitor_suspend(uint64_t i, uint64_t pid)
```
Use the i'th capability to suspend the process `pid`. 

The i'th capability is valid if it is a monitor capability such that `pid` is in the free segment.

**Parameters:**
- `i` - index of monitor capability
- `pid` - PID of the process to suspend

**Returns:**
- `S3K_SUCCESS` - if process was resumed.
- `S3K_PREEMPTED` - if system call was aborted due to timer preemption.
- `S3K_ERR_INVALID_INDEX` - if `i` is invalid (`i >= N_CAP`).
- `S3K_INVALID_PID` - if `pid` is invalid (`pid >= N_PROC`).
- `S3K_ERR_EMPTY` - if the i'th capability slot was empty.
- `S3K_ERR_INVALID_MONITOR` - if i'th capability is invalid.

### s3k_monitor_resume
```c
error_t s3k_monitor_resume(uint64_t i, uint64_t pid)
```
Use the i'th capability to resume the process `pid`. 

The i'th capability is valid if it is a monitor capability such that `pid` is in the free segment.

**Parameters:**
- `i` - index of monitor capability
- `pid` - PID of the process to resume

**Returns:**
- `S3K_SUCCESS` - if capability was deleted.
- `S3K_PREEMPTED` - if system call was aborted due to timer preemption.
- `S3K_ERR_INVALID_INDEX` - if `i` is invalid (`i >= N_CAP`).
- `S3K_INVALID_PID` - if `pid` is invalid (`pid >= N_PROC`).
- `S3K_ERR_EMPTY` - if the i'th capability slot was empty.
- `S3K_ERR_INVALID_MONITOR` - if i'th capability is invalid.

### s3k_monitor_reg_read
```c
error_t s3k_monitor_reg_read(uint64_t i, uint64_t pid, uint64_t reg, uint64_t *val)
```
Use the i'th capability to read from register of process `pid`.

The i'th capability is valid if it is a monitor capability such that `pid` is in the free segment.

**Parameters:**
- `i` - index of monitor capability
- `pid` - PID of the process to resume
- `reg` - index of register
- `val` - buffer to read register value into, 0 if `reg` is invalid

**Returns:**
- `S3K_SUCCESS` - if capability was deleted.
- `S3K_PREEMPTED` - if system call was aborted due to timer preemption.
- `S3K_ERR_INVALID_INDEX` - if `i` is invalid (`i >= N_CAP`).
- `S3K_INVALID_PID` - if `pid` is invalid (`pid >= N_PROC`).
- `S3K_ERR_EMPTY` - if the i'th capability slot was empty.
- `S3K_ERR_INVALID_MONITOR` - if i'th capability is invalid.

### s3k_monitor_reg_write
```c
error_t s3k_monitor_reg_write(uint64_t i, uint64_t pid, uint64_t reg, uint64_t val)
```
Use the i'th capability to write to register of process `pid`. Has no effect if `reg` is invalid.

The i'th capability is valid if it is a monitor capability such that `pid` is in the free segment.

**Parameters:**
- `i` - index of monitor capability
- `pid` - PID of the process to resume
- `reg` - index of register
- `val` - value to write

**Returns:**
- `S3K_SUCCESS` - if capability was deleted.
- `S3K_PREEMPTED` - if system call was aborted due to timer preemption.
- `S3K_ERR_INVALID_INDEX` - if `i` is invalid (`i >= N_CAP`).
- `S3K_INVALID_PID` - if `pid` is invalid (`pid >= N_PROC`).
- `S3K_ERR_EMPTY` - if the i'th capability slot was empty.
- `S3K_ERR_INVALID_MONITOR` - if i'th capability is invalid.

### s3k_monitor_pmp_load
```c
error_t s3k_monitor_pmp_load(uint64_t i, uint64_t pid, uint64_t j, uint64_t k);
```

**Parameters:**
- `i` - index of monitor capability
- `pid` - PID of the process to resume
- `j` - index of pmp capability
- `k` - pmp slot

**Returns:**
- `S3K_SUCCESS` - if PMP capability was loaded.
- `S3K_PREEMPTED` - if system call was aborted due to timer preemption.
- `S3K_ERR_INVALID_INDEX` - if `i` is invalid (`i >= N_CAP`).
- `S3K_ERR_INVALID_INDEX` - if `j` is invalid (`j >= N_CAP`).
- `S3K_ERR_INVALID_SLOT` - if `k` is invalid (`k >= N_PMP`).
- `S3K_ERR_EMPTY` - if the i'th capability slot was empty.
- `S3K_ERR_DST_OCCUPIED` - if the k'th PMP slot of process `pid` was occupied.
- `S3K_ERR_INVALID_MONITOR` - if i'th capability is invalid.
- `S3K_ERR_INVALID_PMP` - if j'th capability is invalid.

### s3k_monitor_pmp_unload
```c
error_t s3k_monitor_pmp_unload();
```

**Parameters:**
- `i` - index of monitor capability
- `pid` - PID of the process to resume
- `j` - index of pmp capability

**Returns:**
- `S3K_SUCCESS` - if PMP capability was loaded.
- `S3K_PREEMPTED` - if system call was aborted due to timer preemption.
- `S3K_ERR_INVALID_INDEX` - if `i` is invalid (`i >= N_CAP`).
- `S3K_ERR_INVALID_INDEX` - if `j` is invalid (`j >= N_CAP`).
- `S3K_ERR_EMPTY` - if the i'th capability slot was empty.
- `S3K_ERR_INVALID_MONITOR` - if i'th capability is invalid.
- `S3K_ERR_INVALID_PMP` - if j'th capability is invalid.

### s3k_monitor_cap_move
```c
error_t s3k_monitor_cap_move();
```

### s3k_sock_send
```c
error_t s3k_client_send();
```

### s3k_sock_call
```c
error_t s3k_client_call();
```

### s3k_sock_recv
```c
error_t s3k_sock_recv();
```

### s3k_sock_reply
```c
error_t s3k_server_reply();
```

### s3k_sock_replyrecv
```c
error_t s3k_server_replyrecv();
```
