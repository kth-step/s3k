extern "C" {
#include "../src/cap.c"
#include "../api/s3k-utils.c"
}
#include <gtest/gtest.h>

namespace
{
class CapTest : public ::testing::Test
{
      protected:
	CapTest()
	{
	}

	~CapTest() override
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

TEST_F(CapTest, MakeTest)
{
	uint64_t hartid = 3, begin = 0x5, end = 0x12;
	union cap cap = cap_time(hartid, begin, end);
	EXPECT_EQ(cap.type, CAPTY_TIME);
	EXPECT_EQ(cap.time.hartid, hartid);
	EXPECT_EQ(cap.time.begin, begin);
	EXPECT_EQ(cap.time.free, begin);
	EXPECT_EQ(cap.time.end, end);

	EXPECT_EQ(cap.time._padd, 0);
}

TEST_F(CapTest, MakeMemory)
{
	uint64_t begin = 0x100, end = 0x200, offset = 0x3, rwx = CAP_RWX;
	union cap cap = cap_memory(begin, end, offset, rwx);
	EXPECT_EQ(cap.type, CAPTY_MEMORY);
	EXPECT_EQ(cap.memory.begin, begin);
	EXPECT_EQ(cap.memory.free, begin);
	EXPECT_EQ(cap.memory.end, end);
	EXPECT_EQ(cap.memory.offset, offset);
	EXPECT_EQ(cap.memory.rwx, rwx);
}

TEST_F(CapTest, MakePMP)
{
	uint64_t addr = 0x85fff, rwx = CAP_RWX;
	union cap cap = cap_pmp(addr, rwx);
	EXPECT_EQ(cap.type, CAPTY_PMP);
	EXPECT_EQ(cap.pmp.addr, addr);
	EXPECT_EQ(cap.pmp.cfg, 0x18 | rwx);
}

TEST_F(CapTest, MakeMonitor)
{
	uint64_t begin = 0x1, end = 0x20;
	union cap cap = cap_monitor(begin, end);
	EXPECT_EQ(cap.type, CAPTY_MONITOR);
	EXPECT_EQ(cap.monitor.begin, begin);
	EXPECT_EQ(cap.monitor.free, begin);
	EXPECT_EQ(cap.monitor.end, end);

	EXPECT_EQ(cap.monitor._padd, 0);
}

TEST_F(CapTest, PmpAddr)
{
	uint64_t begin = 0x100;
	uint64_t end = 0x200;
	uint64_t pmpaddr = pmp_napot_addr(begin, end);
	EXPECT_EQ(begin, pmp_napot_begin(pmpaddr));
	EXPECT_EQ(end, pmp_napot_end(pmpaddr));

	EXPECT_EQ(0x210000, pmp_napot_begin(0x85fff));
	EXPECT_EQ(0x220000, pmp_napot_end(0x85fff));
}

TEST_F(CapTest, MatchAPICap)
{
	union cap cap;
	union s3k_cap s3k_cap;
	cap = cap_time(0x1, 0x1, 0x20);
	s3k_cap = s3k_time(0x1, 0x1, 0x20);
	EXPECT_EQ(cap.raw, s3k_cap.raw);

	cap = cap_memory(0x100, 0x200, 0x3, CAP_RWX);
	s3k_cap = s3k_memory(0x100, 0x200, 0x3, CAP_RWX);
	EXPECT_EQ(cap.raw, s3k_cap.raw);

	cap = cap_pmp(0x100, CAP_RWX);
	s3k_cap = s3k_pmp(0x100, CAP_RWX);
	EXPECT_EQ(cap.raw, s3k_cap.raw);

	cap = cap_monitor(0x1, 0x10);
	s3k_cap = s3k_monitor(0x1, 0x10);
	EXPECT_EQ(cap.raw, s3k_cap.raw);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
