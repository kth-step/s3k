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
	const union cap caps[9] = { cap_pmp(0x20005fff, CAP_RWX),
				    cap_memory(0x0020, 0x8000, 0x10, CAP_RWX),
				    cap_memory(0x0000, 0x0001, 0x2, CAP_RW),
				    cap_time(0, 0, NSLICE),
				    cap_time(1, 0, NSLICE),
				    cap_time(2, 0, NSLICE),
				    cap_time(3, 0, NSLICE),
				    cap_monitor(0, NPROC),
				    cap_channel(0, NCHANNEL) };

      protected:
	SyscallTest()
	{
	}

	~SyscallTest() override
	{
	}

	void SetUp() override
	{
		for (int i = 0; i < NPROC; ++i) {
			processes[i] = { 0 };
			processes[i].pid = i;
		}
		cnode_init();
		cnode_handle_t root = cnode_get_root_handle();
		for (cnode_handle_t i = 0; i < ARRAY_SIZE(caps); ++i) {
			cnode_insert(i, caps[i], root);
		}
	}

	void TearDown() override
	{
		for (cnode_handle_t i = 0; i < NPROC * NCAP; ++i) {
			if (cnode_contains(i)) {
				cnode_delete(i);
			}
		}
	}
};
} // namespace

TEST_F(SyscallTest, GetPid)
{
	for (int i = 0; i < NPROC; ++i) {
		EXPECT_EQ(syscall_getinfo(&processes[i], 0), &processes[i]);
		EXPECT_EQ(processes[i].regs[REG_A0], i);
	}
}

TEST_F(SyscallTest, GetHartID)
{
	for (int i = 0; i < NPROC; ++i) {
		EXPECT_EQ(syscall_getinfo(&processes[i], 1), &processes[i]);
		EXPECT_EQ(processes[i].regs[REG_A0], 0);
	}
}

TEST_F(SyscallTest, GetTime)
{
	for (int i = 0; i < NPROC; ++i) {
		uint64_t before = time_get();
		EXPECT_EQ(syscall_getinfo(&processes[i], 2), &processes[i]);
		uint64_t after = time_get();
		EXPECT_GE(processes[i].regs[REG_A0], before);
		EXPECT_LE(processes[i].regs[REG_A0], after);
	}
}

TEST_F(SyscallTest, GetTimeout)
{
	for (int i = 0; i < NPROC; ++i) {
		uint64_t timeout = time_get();
		timeout_set(0, timeout);
		EXPECT_EQ(syscall_getinfo(&processes[i], 3), &processes[i]);
		EXPECT_EQ(processes[i].regs[REG_A0], timeout);
	}
}
