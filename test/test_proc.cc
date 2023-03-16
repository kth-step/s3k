#include <assert.h>
#include <gtest/gtest.h>

extern "C" {
#include "cap.h"
#include "cnode.h"
#include "csr.h"
#include "proc.h"
}

namespace
{
class ProcTest : public ::testing::Test
{
      private:
	static constexpr union cap caps[]
	    = { CAP_PMP(0x205fff, CAP_RWX), CAP_PMP(0x305fff, CAP_RX),
		CAP_PMP(0x405fff, CAP_RW), CAP_PMP(0x505fff, CAP_R) };

      protected:
	ProcTest()
	{
		cnode_init(caps, ARRAY_SIZE(caps));
		proc_init(0);
	}

	~ProcTest() override
	{
	}

	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};
} // namespace

TEST_F(ProcTest, LoadPmp)
{
	struct proc *proc = proc_get(0);
	proc->regs[REG_PMP] = 0x03020100ull;
	proc_load_pmp(proc);
	EXPECT_EQ(csrr_pmpcfg0(), 0x1F1F1F1F191B1D1Full);
	EXPECT_EQ(csrr_pmpaddr0(), 0x205FFFull);
	EXPECT_EQ(csrr_pmpaddr1(), 0x305FFFull);
	EXPECT_EQ(csrr_pmpaddr2(), 0x405FFFull);
	EXPECT_EQ(csrr_pmpaddr3(), 0x505FFFull);
	EXPECT_EQ(csrr_pmpaddr4(), 0x205FFFull);
	EXPECT_EQ(csrr_pmpaddr5(), 0x205FFFull);
	EXPECT_EQ(csrr_pmpaddr6(), 0x205FFFull);
	EXPECT_EQ(csrr_pmpaddr7(), 0x205FFFull);
}

TEST_F(ProcTest, ProcAcqRel)
{
	struct proc *proc = proc_get(0);
	EXPECT_TRUE(proc_acquire(proc, PS_READY));
	EXPECT_EQ(proc->state, PS_RUNNING);
	proc_release(proc);
	EXPECT_EQ(proc->state, PS_READY);
}

TEST_F(ProcTest, ProcAcqSupRel)
{
	struct proc *proc = proc_get(0);
	EXPECT_TRUE(proc_acquire(proc, PS_READY));
	EXPECT_EQ(proc->state, PS_RUNNING);
	proc_suspend(proc);
	EXPECT_EQ(proc->state, PS_SUSPENDED_BUSY);
	proc_release(proc);
	EXPECT_EQ(proc->state, PS_SUSPENDED);
	EXPECT_FALSE(proc_acquire(proc, PS_READY));
}
