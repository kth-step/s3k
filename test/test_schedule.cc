#include <assert.h>
#include <gtest/gtest.h>

extern "C" {
#include "cap.h"
#include "cnode.h"
#include "csr.h"
#include "proc.h"
#include "schedule.h"
}

namespace
{
class SchedTest : public ::testing::Test
{
      private:
	const union cap caps[4] = {
		cap_pmp(0x205fff, CAP_RWX),
		cap_pmp(0x305fff, CAP_RX),
		cap_pmp(0x405fff, CAP_RW),
		cap_pmp(0x505fff, CAP_R),
	};

      protected:
	SchedTest()
	{
		cnode_init();
		cnode_handle_t root = cnode_get_root_handle();
		for (int i = 0; i < ARRAY_SIZE(caps); ++i) {
			cnode_handle_t handle = cnode_get_handle(0, i);
			cnode_insert(handle, caps[i], root);
		}
		processes[0] = { .pid = 0, .state = PS_READY };
		schedule_init();
	}

	~SchedTest() override
	{
		cnode_init();
		cnode_handle_t root = cnode_get_root_handle();
		for (int i = 0; i < ARRAY_SIZE(caps); ++i) {
			cnode_handle_t handle = cnode_get_handle(0, i);
			cnode_delete(handle);
		}
	}

	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};
} // namespace

TEST_F(SchedTest, SchedUpdate)
{
	struct sched_entry entry;
	schedule_update(0, 2, 0, 8);
	schedule_update(0, 3, 8, 16);
	// First slice [0,8) belongs to process 2
	// Second slice [8,16) belongs to process 3
	// Remainder [16,NSLICE) belongs to process 0

	// First slice
	entry = schedule_get(0, 0);
	EXPECT_EQ(entry.pid, 2);
	EXPECT_EQ(entry.len, 8);

	entry = schedule_get(0, 1);
	EXPECT_EQ(entry.pid, 2);
	EXPECT_EQ(entry.len, 7);

	entry = schedule_get(0, 7);
	EXPECT_EQ(entry.pid, 2);
	EXPECT_EQ(entry.len, 1);

	// Second slice
	entry = schedule_get(0, 8);
	EXPECT_EQ(entry.pid, 3);
	EXPECT_EQ(entry.len, 8);

	entry = schedule_get(0, 9);
	EXPECT_EQ(entry.pid, 3);
	EXPECT_EQ(entry.len, 7);

	entry = schedule_get(0, 15);
	EXPECT_EQ(entry.pid, 3);
	EXPECT_EQ(entry.len, 1);

	// Remaining
	entry = schedule_get(0, 16);
	EXPECT_EQ(entry.pid, 0);
	EXPECT_EQ(entry.len, NSLICE - 16);

	entry = schedule_get(0, 17);
	EXPECT_EQ(entry.pid, 0);
	EXPECT_EQ(entry.len, NSLICE - 17);

	entry = schedule_get(0, NSLICE - 1);
	EXPECT_EQ(entry.pid, 0);
	EXPECT_EQ(entry.len, 1);
}

TEST_F(SchedTest, SchedDelete)
{
	struct sched_entry entry;
	// Create slice [0,8) for proc 2
	// Delete the slice
	schedule_update(0, 2, 0, 8);
	schedule_delete(0, 0, 8);

	// Deleted slice, pid == 0xFF means NONE/INVALID
	entry = schedule_get(0, 0);
	EXPECT_EQ(entry.pid, 0xFF);
	EXPECT_EQ(entry.len, 8);

	entry = schedule_get(0, 1);
	EXPECT_EQ(entry.pid, 0xFF);
	EXPECT_EQ(entry.len, 7);

	entry = schedule_get(0, 7);
	EXPECT_EQ(entry.pid, 0xFF);
	EXPECT_EQ(entry.len, 1);

	// Remaining
	entry = schedule_get(0, 8);
	EXPECT_EQ(entry.pid, 0);
	EXPECT_EQ(entry.len, NSLICE - 8);

	entry = schedule_get(0, 17);
	EXPECT_EQ(entry.pid, 0);
	EXPECT_EQ(entry.len, NSLICE - 17);

	entry = schedule_get(0, NSLICE - 1);
	EXPECT_EQ(entry.pid, 0);
	EXPECT_EQ(entry.len, 1);
}
