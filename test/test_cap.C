extern "C" {
#include "../api/s3k.h"
#include "cap.h"
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
	union cap cap = CAP_TIME(hartid, begin, end);
	EXPECT_EQ(cap.type, CAPTY_TIME);
	EXPECT_EQ(cap.time.hartid, hartid);
	EXPECT_EQ(cap.time.begin, begin);
	EXPECT_EQ(cap.time.free, begin);
	EXPECT_EQ(cap.time.end, end);

	EXPECT_EQ(cap.time.unused, 0);
}

TEST_F(CapTest, MakeMemory)
{
	uint64_t begin = 0x100, end = 0x200, offset = 0x3, rwx = CAP_RWX;
	union cap cap = CAP_MEMORY(begin, end, offset, rwx);
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
	union cap cap = CAP_PMP(addr, rwx);
	EXPECT_EQ(cap.type, CAPTY_PMP);
	EXPECT_EQ(cap.pmp.addr, addr);
	EXPECT_EQ(cap.pmp.cfg, 0x18 | rwx);
}

TEST_F(CapTest, MakeMonitor)
{
	uint64_t begin = 0x1, end = 0x20;
	union cap cap = CAP_MONITOR(begin, end);
	EXPECT_EQ(cap.type, CAPTY_MONITOR);
	EXPECT_EQ(cap.monitor.begin, begin);
	EXPECT_EQ(cap.monitor.free, begin);
	EXPECT_EQ(cap.monitor.end, end);

	EXPECT_EQ(cap.monitor.unused, 0);
}

TEST_F(CapTest, MakeChannel)
{
	uint64_t begin = 0x1, end = 0x20;
	union cap cap = CAP_CHANNEL(begin, end);
	EXPECT_EQ(cap.type, CAPTY_CHANNEL);
	EXPECT_EQ(cap.channel.begin, begin);
	EXPECT_EQ(cap.channel.free, begin);
	EXPECT_EQ(cap.channel.end, end);

	EXPECT_EQ(cap.channel.unused, 0);
}

TEST_F(CapTest, MakeSocket)
{
	uint64_t channel = 0x3, tag = 0x20;
	union cap cap = CAP_SOCKET(channel, tag);
	EXPECT_EQ(cap.type, CAPTY_SOCKET);
	EXPECT_EQ(cap.socket.channel, channel);
	EXPECT_EQ(cap.socket.tag, tag);

	EXPECT_EQ(cap.socket.unused, 0);
}

TEST_F(CapTest, TimeDerive)
{
	EXPECT_FALSE(cap_time_derive(CAP_TIME(3, 1, 20), CAP_TIME(3, 0, 20)));
	EXPECT_FALSE(cap_time_derive(CAP_TIME(2, 1, 20), CAP_TIME(3, 1, 20)));
	for (unsigned int i = 0; i < 255; i++) {
		EXPECT_TRUE(
		    cap_time_derive(CAP_TIME(i, 1, 20), CAP_TIME(i, 1, 20)));
		EXPECT_FALSE(
		    cap_time_derive(CAP_TIME(i, 5, 20), CAP_TIME(i, 4, 10)));
		EXPECT_FALSE(
		    cap_time_derive(CAP_TIME(i, 5, 20), CAP_TIME(i, 6, 10)));
	}
}

TEST_F(CapTest, TimeParent)
{
	EXPECT_FALSE(cap_time_parent(CAP_TIME(3, 1, 20), CAP_TIME(3, 0, 20)));
	EXPECT_FALSE(cap_time_parent(CAP_TIME(2, 1, 20), CAP_TIME(3, 1, 20)));
	for (unsigned int i = 0; i < 255; i++) {
		EXPECT_TRUE(
		    cap_time_parent(CAP_TIME(i, 1, 20), CAP_TIME(i, 1, 20)));
		EXPECT_FALSE(
		    cap_time_parent(CAP_TIME(i, 5, 20), CAP_TIME(i, 4, 10)));
	}
}

TEST_F(CapTest, MemoryDerive)
{
	// RWX
	EXPECT_TRUE(cap_memory_derive(CAP_MEMORY(0x0, 0x3000, 8, CAP_RWX),
				      CAP_MEMORY(0x0, 0x2000, 8, CAP_RWX)));
	EXPECT_TRUE(cap_memory_derive(CAP_MEMORY(0x0, 0x3000, 8, CAP_RWX),
				      CAP_MEMORY(0x0, 0x2000, 8, CAP_RX)));
	EXPECT_TRUE(cap_memory_derive(CAP_MEMORY(0x0, 0x3000, 8, CAP_RWX),
				      CAP_MEMORY(0x0, 0x2000, 8, CAP_RW)));
	EXPECT_TRUE(cap_memory_derive(CAP_MEMORY(0x0, 0x3000, 8, CAP_RWX),
				      CAP_MEMORY(0x0, 0x2000, 8, CAP_R)));
	// RX
	EXPECT_TRUE(cap_memory_derive(CAP_MEMORY(0x0, 0x3000, 8, CAP_RX),
				      CAP_MEMORY(0x0, 0x2000, 8, CAP_RX)));
	EXPECT_TRUE(cap_memory_derive(CAP_MEMORY(0x0, 0x3000, 8, CAP_RX),
				      CAP_MEMORY(0x0, 0x2000, 8, CAP_R)));
	// RW
	EXPECT_TRUE(cap_memory_derive(CAP_MEMORY(0x0, 0x3000, 8, CAP_RW),
				      CAP_MEMORY(0x0, 0x2000, 8, CAP_RW)));
	EXPECT_TRUE(cap_memory_derive(CAP_MEMORY(0x0, 0x3000, 8, CAP_RW),
				      CAP_MEMORY(0x0, 0x2000, 8, CAP_R)));
	// R
	EXPECT_TRUE(cap_memory_derive(CAP_MEMORY(0x0, 0x3000, 8, CAP_R),
				      CAP_MEMORY(0x0, 0x2000, 8, CAP_R)));

	// EQ
	EXPECT_TRUE(cap_memory_derive(CAP_MEMORY(0x100, 0x200, 8, CAP_RWX),
				      CAP_MEMORY(0x100, 0x200, 8, CAP_RWX)));
	// Larger
	EXPECT_FALSE(cap_memory_derive(CAP_MEMORY(0x100, 0x3000, 8, CAP_RWX),
				       CAP_MEMORY(0x0, 0x2000, 8, CAP_RWX)));
	EXPECT_FALSE(cap_memory_derive(CAP_MEMORY(0x100, 0x3000, 8, CAP_RWX),
				       CAP_MEMORY(0x100, 0x3001, 8, CAP_RWX)));
	EXPECT_FALSE(cap_memory_derive(CAP_MEMORY(0x100, 0x3000, 8, CAP_RWX),
				       CAP_MEMORY(0x99, 0x3001, 8, CAP_RWX)));
	// OFFSET NOT EQ
	EXPECT_FALSE(cap_memory_derive(CAP_MEMORY(0x100, 0x200, 8, CAP_RWX),
				       CAP_MEMORY(0x100, 0x200, 7, CAP_RWX)));
	// Bad permissions
	EXPECT_FALSE(cap_memory_derive(CAP_MEMORY(0x0, 0x3000, 8, CAP_R),
				      CAP_MEMORY(0x0, 0x2000, 8, CAP_RW)));
	EXPECT_FALSE(cap_memory_derive(CAP_MEMORY(0x0, 0x3000, 8, CAP_R),
				      CAP_MEMORY(0x0, 0x2000, 8, CAP_RWX)));
	EXPECT_FALSE(cap_memory_derive(CAP_MEMORY(0x0, 0x3000, 8, CAP_R),
				      CAP_MEMORY(0x0, 0x2000, 8, CAP_RX)));
	EXPECT_FALSE(cap_memory_derive(CAP_MEMORY(0x0, 0x3000, 8, CAP_RW),
				      CAP_MEMORY(0x0, 0x2000, 8, CAP_RWX)));
	EXPECT_FALSE(cap_memory_derive(CAP_MEMORY(0x0, 0x3000, 8, CAP_RW),
				      CAP_MEMORY(0x0, 0x2000, 8, CAP_RX)));
	EXPECT_FALSE(cap_memory_derive(CAP_MEMORY(0x0, 0x3000, 8, CAP_RX),
				      CAP_MEMORY(0x0, 0x2000, 8, CAP_RWX)));
	EXPECT_FALSE(cap_memory_derive(CAP_MEMORY(0x0, 0x3000, 8, CAP_RX),
				      CAP_MEMORY(0x0, 0x2000, 8, CAP_RW)));
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
