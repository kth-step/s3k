#include "proc.h"

void syscall_recv(struct proc *proc, uint64_t recv_idx)
{
}

void syscall_send(struct proc *proc, uint64_t send_idx, uint64_t msg0, uint64_t msg1,
		  uint64_t cap0, uint64_t cap1, uint64_t yield)
{
}

void syscall_sendrecv(struct proc *proc, uint64_t send_idx, uint64_t recv_idx,
		      uint64_t msg0, uint64_t msg1, uint64_t cap0,
		      uint64_t cap1)
{
}
