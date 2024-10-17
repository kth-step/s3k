#include "kern/cap.h"
#include "kern/error.h"
#include "kern/sched.h"
#include "kern/utils.h"

struct {
	int prev, next;
	Cap cap;
} ctable[];

static inline Word CapIdxPid(Word i)
{
	return i / S3K_CAP_CNT;
}

Word CapRead(Word i, Cap *cap)
{
	*cap = ctable[i].cap;
	return SUCCESS;
}

Word CapMove(Word src, Word dst)
{
	Cap cap = ctable[src].cap;
	Word prev = ctable[src].prev;
	Word next = ctable[src].next;

	if (cap.type == CAPTY_NONE)
		return ERR_INVCAP;

	ctable[dst].cap = cap;
	ctable[dst].prev = prev;
	ctable[dst].next = next;
	ctable[next].prev = dst;
	ctable[prev].next = dst;

	ctable[src].cap = CAP_NULL;

	if (CapIdxPid(src) != CapIdxPid(dst)) {
		if (cap.type == CAPTY_TIME) {
			SchedUpdate(CapIdxPid(dst), cap.time.hart, cap.time.mark, cap.time.end);
		}
		if (cap.type == CAPTY_SERVER)
			IpcClear(cap.server.channel, cap.server.channel + 1);
	}

	return SUCCESS;
}

Word CapDelete(Word i)
{
	Cap cap = ctable[i].cap;
	Word prev = ctable[i].prev;
	Word next = ctable[i].next;

	if (cap.type == CAPTY_NONE)
		return ERR_INVCAP;

	ctable[prev].next = next;
	ctable[next].prev = prev;

	ctable[i].cap = CAP_NULL;

	if (cap.type == CAPTY_TIME) {
		SchedDelete(cap.time.hart, cap.time.mark, cap.time.end);
	} else if (cap.type == CAPTY_SERVER) {
		IpcClear(cap.server.channel, cap.server.channel + 1);
	}

	return SUCCESS;
}

static Word CapDerivable(Cap p, Cap c)
{
	if (p.type == CAPTY_TIME && c.type == CAPTY_TIME) {
		return c.time.mark == c.time.begin && c.time.begin < c.time.end && p.time.hart == c.time.hart
		       && p.time.mark <= c.time.begin && c.time.end <= p.time.end;
	} else if (p.type == CAPTY_MEMORY && c.type == CAPTY_MEMORY) {
		return c.mem.mark == c.mem.begin && c.mem.begin < c.mem.end && c.mem.lock == 0 && p.mem.lock == 0
		       && p.mem.tag == c.mem.tag && p.mem.begin <= c.mem.begin && c.mem.end <= p.mem.end
		       && BitSubset(c.mem.rwx, p.mem.rwx);
	} else if (p.type == CAPTY_MEMORY && c.type == CAPTY_PMP) {
		return CapMemBegin(p) <= CapPmpBegin(c) && CapPmpEnd(c) <= CapMemEnd(p)
		       && BitSubset(c.pmp.rwx, p.mem.rwx);
	} else if (p.type == CAPTY_PMP && c.type == CAPTY_PMP) {
		return CapPmpBegin(p) <= CapPmpBegin(c) && CapPmpEnd(c) <= CapPmpEnd(p)
		       && BitSubset(c.pmp.rwx, p.pmp.rwx);
	} else if (p.type == CAPTY_MONITOR && p.type == CAPTY_MONITOR) {
		return c.mon.mark == c.mon.begin && c.mon.begin < c.mon.end && p.mon.mark <= c.mon.begin
		       && c.mon.end <= p.mon.end;
	} else if (p.type == CAPTY_CHANNEL && c.type == CAPTY_CHANNEL) {
		return c.chan.mark == c.chan.begin && c.chan.begin < c.chan.end && p.chan.mark <= c.chan.begin
		       && c.chan.end <= p.chan.end;
	} else if (p.type == CAPTY_CHANNEL && c.type == CAPTY_SERVER) {
		return p.chan.mark <= c.server.channel && c.server.channel < c.chan.end;
	} else if (p.type == CAPTY_SERVER && c.type == CAPTY_CLIENT) {
		return p.server.channel == c.client.channel && p.server.mode == c.client.mode
		       && p.server.permission == c.server.permission;
	}
	return 0;
}

Word CapDerive(Word src, Word dst, Cap new_cap)
{
	Cap cap = ctable[src].cap;
	Word next = ctable[src].next;

	if (cap.type == CAPTY_NONE || cap.type == CAPTY_PMP || cap.type == CAPTY_CLIENT)
		return ERR_INVCAP;
	if (ctable[dst].cap.type != CAPTY_NONE)
		return ERR_EXISTS;
	if (!CapDerivable(cap, new_cap))
		return ERR_INVDRV;

	// Update parent capability and associated resources.
	if (cap.type == CAPTY_TIME && new_cap.type == CAPTY_TIME) {
		cap.time.mark = new_cap.time.begin;
		SchedUpdate(CapIdxPid(dst), new_cap.time.hart, new_cap.time.begin, new_cap.time.end);
	} else if (cap.type == CAPTY_MEMORY && new_cap.type == CAPTY_MEMORY) {
		cap.mem.mark = new_cap.mem.begin;
	} else if (cap.type == CAPTY_MEMORY && new_cap.type == CAPTY_MEMORY) {
		cap.mem.lock = 1;
	} else if (cap.type == CAPTY_MONITOR && new_cap.type == CAPTY_MONITOR) {
		cap.mon.mark = new_cap.mon.begin;
	} else if (cap.type == CAPTY_CHANNEL && new_cap.type == CAPTY_CHANNEL) {
		cap.chan.mark = new_cap.chan.begin;
	} else if (cap.type == CAPTY_CHANNEL && new_cap.type == CAPTY_SERVER) {
		cap.chan.mark = new_cap.server.channel + 1;
	}

	ctable[src].next = dst;
	ctable[src].cap = cap;

	// Insert new capability
	ctable[dst].prev = src;
	ctable[dst].next = next;
	ctable[dst].cap = new_cap;
}

Word CapRevokable(Cap p, Cap c)
{
	if (p.type == CAPTY_TIME && c.type == CAPTY_TIME) {
		return p.time.hart == c.time.hart && c.time.end <= p.time.end;
	} else if (p.type == CAPTY_MEMORY && c.type == CAPTY_MEMORY) {
		return CapMemEnd(c) <= CapMemEnd(p);
	} else if (p.type == CAPTY_MEMORY && c.type == CAPTY_PMP) {
		return CapPmpBegin(c) <= CapMemEnd(p);
	} else if (p.type == CAPTY_MONITOR && c.type == CAPTY_MONITOR) {
		return c.mon.end <= p.mon.end;
	} else if (p.type == CAPTY_CHANNEL && c.type == CAPTY_CHANNEL) {
		return c.chan.end <= p.chan.end;
	} else if (p.type == CAPTY_CHANNEL && c.type == CAPTY_SERVER) {
		return c.server.channel < p.chan.end;
	} else if (p.type == CAPTY_CHANNEL && c.type == CAPTY_CLIENT) {
		return c.client.channel < p.chan.end;
	}
	return false;
}

Word CapRevoke(Word i)
{
	Cap cap = ctable[i].cap;

	if (cap.type == CAPTY_NONE || cap.type == CAPTY_PMP || cap.type == CAPTY_CLIENT)
		return ERR_INVCAP;

	bool done = false;
	Word next = ctable[i].next;
	Cap next_cap = ctable[next].cap;

	while (CapRevokable(cap, next_cap) && i != next) {
		// If parent of next capability, delete it.
		if (Preempted())
			goto preempted;
		// Update parent.
		if (cap.type == CAPTY_TIME && next_cap.type == CAPTY_TIME) {
			cap.time.mark = next_cap.time.mark;
		} else if (cap.type == CAPTY_MEMORY && next_cap.type == CAPTY_MEMORY) {
			cap.mem.mark = next_cap.mem.mark;
			cap.mem.lock = next_cap.mem.lock;
		} else if (cap.type == CAPTY_MEMORY && next_cap.type == CAPTY_PMP) {
			// nothing
		} else if (cap.type == CAPTY_MONITOR && next_cap.type == CAPTY_MONITOR) {
			cap.mon.mark = next_cap.mon.mark;
		} else if (cap.type == CAPTY_CHANNEL && next_cap.type == CAPTY_CHANNEL) {
			cap.chan.mark = next_cap.chan.mark;
		} else if (cap.type == CAPTY_CHANNEL && next_cap.type == CAPTY_SERVER) {
			cap.chan.mark = next_cap.server.channel + 1;
		} else if (cap.type == CAPTY_CHANNEL && next_cap.type == CAPTY_CLIENT) {
			cap.chan.mark = next_cap.client.channel + 1;
		}
		// Delete the capability
		ctable[next].cap = CAP_NULL;
		next = ctable[next].next;
		next_cap = ctable[next].cap;
	}

	done = true;

	if (cap.type == CAPTY_TIME) {
		cap.time.mark = cap.time.begin;
	} else if (cap.type == CAPTY_MEMORY) {
		cap.mem.mark = cap.mem.begin;
		cap.mem.lock = 0;
	} else if (cap.type == CAPTY_MONITOR) {
		cap.mon.mark = cap.mon.begin;
	} else if (cap.type == CAPTY_CHANNEL) {
		cap.chan.mark = cap.chan.begin;
	}

preempted:
	// Interrupted.
	ctable[i].cap = cap;
	ctable[i].next = next;
	ctable[next].prev = i;

	if (cap.type == CAPTY_TIME)
		SchedUpdate(CapIdxPid(i), cap.time.hart, cap.time.mark, cap.time.end);
	if (cap.type == CAPTY_CHANNEL)
		IpcClear(cap.chan.mark, cap.chan.end);

	return done ? SUCCESS : ERR_PREEMPT;
}

bool cap_monitor_valid_pid(Cap cap, Word pid)
{
	return pid >= cap.mon.begin && pid < cap.mon.end;
}

Error CapMonitorSuspend(Word mon_idx, Word pid)
{
	Cap cap = ctable[mon_idx].cap;
	if (cap.type != CAPTY_MONITOR)
		return ERR_INVCAP;
	if (!cap_monitor_valid_pid(cap, pid))
		return ERR_INVCAP;
	ProcSuspend(ProcGet(pid));
	return SUCCESS;
}

Error CapMonitorResume(Word mon_idx, Word pid)
{
	Cap cap = ctable[mon_idx].cap;
	if (cap.type != CAPTY_MONITOR)
		return ERR_INVCAP;
	if (!cap_monitor_valid_pid(cap, pid))
		return ERR_INVCAP;
	ProcResume(ProcGet(pid));
	return SUCCESS;
}

Error CapMonitorGetState(Word mon_idx, Word pid, ProcState *state)
{
	Cap cap = ctable[mon_idx].cap;
	if (cap.type != CAPTY_MONITOR)
		return ERR_INVCAP;
	if (!cap_monitor_valid_pid(cap, pid))
		return ERR_INVCAP;
	*state = ProcGetState(ProcGet(pid));
	return SUCCESS;
}

Error CapMonitorRegRead(Word mon_idx, Word pid, Word regnr, Word *val)
{
	Cap cap = ctable[mon_idx].cap;
	if (cap.type != CAPTY_MONITOR)
		return ERR_INVCAP;
	if (!CapMonitorValidPid(cap, pid))
		return ERR_INVCAP;
	if (regnr >= REG_COUNT)
		return ERR_INVARG;
	Proc *p = ProcGet(pid);
	Word *regs = (Word *)&p->regs;
	*val = regs[regnr];
	return SUCCESS;
}

Error CapMonitorRegWrite(Word mon_idx, Word pid, Word regnr, Word val)
{
	Cap cap = ctable[mon_idx].cap;
	if (cap.type != CAPTY_MONITOR)
		return ERR_INVCAP;
	if (!cap_monitor_valid_pid(cap, pid))
		return ERR_INVCAP;
	if (regnr >= REG_COUNT)
		return ERR_INVARG;
	Proc *p = ProcGet(pid);
	Word *regs = (Word *)&p->regs;
	regs[regnr] = val;
	return SUCCESS;
}

Error CapMonitorCapRead(Word mon_idx, Word read_idx, Cap *buf_cap)
{
	Cap cap = ctable[mon_idx].cap;
	Word pid = CapIdxPid(read_idx);
	if (cap.type != CAPTY_MONITOR)
		return ERR_INVCAP;
	if (!cap_monitor_valid_pid(cap, pid))
		return ERR_INVCAP;
	*buf_cap = ctable[read_idx].cap;
	return SUCCESS;
}

err_t cap_monitor_cap_move(cidx_t mon, cidx_t src, cidx_t dst)
{
	cap_t cap = ctable[mon_idx].cap;
	pid_t mon_pid = cap_pid(mon);
	pid_t src_pid = cap_pid(src);
	pid_t dst_pid = cap_pid(dst);
	if (cap.type != CAPTY_MONITOR)
		return ERR_INVCAP;
	if (src_pid != mon_pid && !cap_monitor_valid_pid(cap, src_pid))
		return ERR_INVCAP;
	if (dst_pid != mon_pid && !cap_monitor_valid_pid(cap, dst_pid))
		return ERR_INVCAP;
	return cap_move(src, dst);
}

err_t cap_monitor_yield(cidx_t mon, pid_t pid)
{
	if (cap.type != CAPTY_MONITOR)
		return ERR_INVCAP;
	if (!cap_monitor_valid_pid(cap, pid))
		return ERR_INVCAP;
	proc_t *proc = proc_get(pid);
	if (proc_acquire(proc)) {
		proc_release(proc);
		return SUCCESS;
	}
	return ERR_INVSTATE;
}

err_t cap_server_send(cidx_t srv, uint64_t data[4])
{
	if (cap.type != CAPTY_SERVER)
		return ERR_INVCAP;
	return SUCCESS;
}

err_t cap_server_recv(cidx_t srv)
{
	if (cap.type != CAPTY_SERVER)
		return ERR_INVCAP;
	return SUCCESS;
}

err_t cap_server_sendrecv(cidx_t srv, uint64_t data[4])
{
	if (cap.type != CAPTY_SERVER)
		return ERR_INVCAP;
	return SUCCESS;
}

err_t cap_client_send(cidx_t clt, uint64_t data[4])
{
	if (cap.type != CAPTY_CLIENT)
		return ERR_INVCAP;
	return SUCCESS;
}

err_t cap_client_recv(cidx_t clt)
{
	if (cap.type != CAPTY_CLIENT)
		return ERR_INVCAP;
	return SUCCESS;
}

err_t cap_client_sendrecv(cidx_t clt, uint64_t data[4])
{
	if (cap.type != CAPTY_CLIENT)
		return ERR_INVCAP;
	return SUCCESS;
}
