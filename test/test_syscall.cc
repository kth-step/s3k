#include <assert.h>
#include <gtest/gtest.h>

extern "C" {
#include "cap.h"
#include "cnode.h"
#include "csr.h"
#include "proc.h"
#include "syscall.h"
#include "timer.h"
}

namespace
{
class SyscallTest : public ::testing::Test
{
      private:
	static constexpr union cap caps[]
	    = { CAP_PMP(0x20005FFF, CAP_RWX),
		CAP_MEMORY(0x0020, 0X8000, 0x10, CAP_RWX),
		CAP_MEMORY(0x0000, 0X0001, 0x2, CAP_RW),
		CAP_TIME(0, 0, NSLICE),
		CAP_TIME(1, 0, NSLICE),
		CAP_TIME(2, 0, NSLICE),
		CAP_TIME(3, 0, NSLICE),
		CAP_MONITOR(0, NPROC),
		CAP_CHANNEL(0, NCHANNEL) };

      protected:
	SyscallTest()
	{
	}

	~SyscallTest() override
	{
	}

	void SetUp() override
	{
		cnode_init(caps, ARRAY_SIZE(caps));
		proc_init(0);
	}

	void TearDown() override
	{
	}
};
} // namespace

TEST_F(SyscallTest, GetPid)
{
	for (int i = 0; i < NPROC; ++i) {
		struct proc *proc = proc_get(i);
		syscall_getinfo(proc, 0);
		EXPECT_EQ(proc->regs[REG_A0], i);
	}
}

TEST_F(SyscallTest, GetHartID)
{
	for (int i = 0; i < NPROC; ++i) {
		struct proc *proc = proc_get(i);
		syscall_getinfo(proc, 1);
		EXPECT_EQ(proc->regs[REG_A0], 0);
	}
}

TEST_F(SyscallTest, GetTime)
{
	for (int i = 0; i < NPROC; ++i) {
		struct proc *proc = proc_get(i);
		uint64_t before = time_get();
		syscall_getinfo(proc, 2);
		uint64_t after = time_get();
		EXPECT_GE(proc->regs[REG_A0], before);
		EXPECT_LE(proc->regs[REG_A0], after);
	}
}

TEST_F(SyscallTest, GetTimeout)
{
	for (int i = 0; i < NPROC; ++i) {
		struct proc *proc = proc_get(i);
		uint64_t timeout = time_get();
		timeout_set(0, timeout);
		syscall_getinfo(proc, 3);
		EXPECT_EQ(proc->regs[REG_A0], timeout);
	}
}
