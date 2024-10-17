/* See LICENSE file for copyright and license details. */
#include "drivers/time.h"
#include "kern/cap.h"
#include "kern/csr.h"
#include "kern/error.h"
#include "kern/sched.h"
#include "kern/syscall.h"
#include "kern/trap.h"
#include "kern/types.h"

static Proc *HandleInvArg(Proc *p);

static Proc *HandleGetPid(Proc *p);
static Proc *HandleRegRead(Proc *p);
static Proc *HandleRegWrite(Proc *p);
static Proc *HandleSync(Proc *p);
static Proc *HandleCapRead(Proc *p);
static Proc *HandleCapMove(Proc *p);
static Proc *HandleCapDelete(Proc *p);
static Proc *HandleCapDerive(Proc *p);
static Proc *HandleCapRevoke(Proc *p);

typedef Proc *(*Handler)(Proc *p);

static const Handler handlers[] = {
    HandleGetPid,  HandleRegRead,   HandleRegWrite,  HandleSync,      HandleCapRead,
    HandleCapMove, HandleCapDelete, HandleCapDerive, HandleCapRevoke,
};

Proc *SyscallHandler(Proc *p)
{
	Word syscall_nr = p->regs.t0;
	if (syscall_nr < ARRAY_SIZE(handlers))
		return handlers[syscall_nr](p);
	return HandleInvArg(p);
}

static Bool ValidCapIdx(Word i)
{
	return i < S3K_CAP_CNT;
}

static Bool ValidPmpSlot(Word slot)
{
	return slot < S3K_PMP_CNT;
}

static Bool ValidPid(Word pid)
{
	return pid < S3K_PROC_CNT;
}

static Bool ValidReg(Word reg)
{
	return reg < REGISTER_COUNT;
}

Proc *HandleInvArg(Proc *p)
{
	p->regs.t0 = ERR_INVARG;
	return p;
}

Proc *HandleGetPid(Proc *p)
{
	p->regs.t0 = SUCCESS;
	p->regs.a0 = p->pid;
	return p;
}

Proc *HandleRegRead(Proc *p)
{
	if (!ValidReg(p->regs.a0))
		return HandleInvArg(p);

	Word reg = p->regs.a0;
	Word *regs = (Word *)&p->regs;
	p->regs.a0 = regs[reg];
	p->regs.t0 = SUCCESS;
	return p;
}

Proc *HandleRegWrite(Proc *p)
{
	if (!ValidReg(p->regs.a0))
		return HandleInvArg(p);

	Word reg = p->regs.a0;
	Word value = p->regs.a1;
	Word *regs = (Word *)&p->regs;
	regs[reg] = value;
	p->regs.t0 = SUCCESS;
	return p;
}

Proc *HandleSync(Proc *p)
{
	return NULL;
}

Proc *HandleCapRead(Proc *p)
{
	if (!ValidCapIdx(p->regs.a0))
		return HandleInvArg(p);

	Word i = CapIdx(p->pid, p->regs.a0);
	Cap *buf = (Cap *)&p->regs.a0;
	p->regs.t0 = CapRead(i, buf);
	return p;
}

Proc *HandleCapMove(Proc *p)
{
	if (!ValidCapIdx(p->regs.a0) || !ValidCapIdx(p->regs.a0))
		return HandleInvArg(p);
	Word src = CapIdx(p->pid, p->regs.a0);
	Word dst = CapIdx(p->pid, p->regs.a1);
	p->regs.t0 = CapMove(src, dst);
	return p;
}

Proc *HandleCapDelete(Proc *p)
{
	if (!ValidCapIdx(p->regs.a0))
		return HandleInvArg(p);
	Word i = CapIdx(p->pid, p->regs.a0);
	current->regs.t0 = CapDelete(i);
	return p;
}

Proc *HandleCapDerive(Proc *p)
{
	if (!ValidCapIdx(p->regs.a0) || !ValidCapIdx(p->regs.a0))
		return HandleInvArg(p);
	Word src = CapIdx(p->pid, p->regs.a0);
	Word dst = CapIdx(p->pid, p->regs.a1);
	Cap cap = (Cap){.raw = p->regs.a2};
	current->regs.t0 = CapDerive(src, dst, cap);
}

Proc *HandleCapRevoke(Proc *p)
{
	if (!ValidCapIdx(p->regs.a0))
		return HandleInvArg(p);
	Word i = CapIdx(p->pid, p->regs.a0);
	current->regs.t0 = CapRevoke(i);
	return p;
}
