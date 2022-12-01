# S3K - API

## Basic API

### System calls (Basic)

- `void s3k_get_pid()` - Get the process's PID.
- `uint64_t s3k_read_reg(register_number)` - Read a virtual register.
- `void s3k_write_reg(register_number, value)` - Write to a virtual register.
- `void s3k_yield()` - Yield the remainder of the time slice (TODO: rename the function?).
- `cap_t s3k_read_cap(i)` - Read capability from slot `i`.
- `uint64_t s3k_move_cap(i, j)` - Move a capability in slot `i` to slot `j`.
- `uint64_t s3k_delete_cap(i)` -  Delete capability in slot `i`.
- `uint64_t s3k_revoke_cap(i) - Delete all children of capability in slot `i`.
- `uint64_t s3k_derive_cap(i, j, cap) - Derive capability `cap` from capability in slot `i` and place in slot `j`.

### System calls (Capability invocation)
The following system calls are pseudo system calls implemented on `s3k_invoke_cap(i, a1, a2, a3, a4, a5, a6, a7)`.

**Supervisor Invocations,** the `i` of the following system calls should point at a supervisor capability.
- `uint64_t s3k_supervisor_suspend(i, pid)` - Suspend process `pid`.
- `uint64_t s3k_supervisor_resume(i, pid)` - Resume process `pid`.
- `uint64_t s3k_supervisor_get_state(i, pid)` - Get state of process `pid`.
- `uint64_t s3k_supervisor_read_reg(i, pid, register_number)` - Reads a virtual register of process `pid`. (Req. process `pid` suspended).
- `uint64_t s3k_supervisor_write_reg(i, pid, register_number, value)` - Write to virtual register of process `pid`. (Req. process `pid` suspended).
- `uint64_t s3k_supervisor_read_cap(i, pid, j)` - Read capability `j` of process `pid`. (Req. process `pid` suspended).
- `uint64_t s3k_supervisor_give_cap(i, pid, j, k)` - Give capability `j` to process `pid`, placing it in slot `k`. (Req. process `pid` suspended).
- `uint64_t s3k_supervisor_take_cap(i, pid, j, k)` - Take capability `j` from process `pid`, placing it in slot `k`. (Req. process `pid` suspended).
