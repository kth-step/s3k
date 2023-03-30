extern "C" {
#include "../api/s3k.h"
#include "cap.h"
}
#include <gtest/gtest.h>

namespace
{
class ApiTest : public ::testing::Test
{
      protected:
	ApiTest()
	{
	}

	~ApiTest() override
	{
	}

	void SetUp() override
	{
	}

	void TearDown() override
	{
	}

	unsigned long long llrand()
	{
		unsigned long long r = 0;

		for (int i = 0; i < 5; ++i) {
			r = (r << 15) | (rand() & 0x7FFF);
		}

		return r & 0xFFFFFFFFFFFFFFFFULL;
	}
};
} // namespace

TEST_F(ApiTest, TimeCap)
{
	union cap cap;
	union s3k_cap s3k_cap;
	// Test if time capabilities match
	for (int i = 0; i < 100; ++i) {
		uint64_t begin = llrand();
		uint64_t end = llrand();
		uint64_t hartid = llrand();
		cap = CAP_TIME(hartid, begin, end);
		s3k_cap = s3k_time(hartid, begin, end);
		ASSERT_EQ(cap.raw, s3k_cap.raw);
	}
}

TEST_F(ApiTest, TimeParent)
{
	union cap cap, cap_child;
	union s3k_cap s3k_cap, s3k_cap_child;

	// Test if parent predicate match
	int children = 0;
	while (children < 120) {
		uint64_t raw1 = llrand();
		uint64_t raw2 = llrand();
		cap.raw = raw1;
		cap_child.raw = raw2;
		s3k_cap.raw = raw1;
		s3k_cap_child.raw = raw2;
		ASSERT_EQ(cap_time_parent(cap, cap_child),
			  s3k_time_parent(s3k_cap, s3k_cap_child));
		if (cap_time_parent(cap, cap_child))
			children++;
	}
}

TEST_F(ApiTest, MemoryCap)
{
	// Test if memory capabilities match
	union cap cap;
	union s3k_cap s3k_cap;
	for (int i = 0; i < 100; ++i) {
		uint64_t begin = llrand() % 0x8001;
		uint64_t end = llrand() % 0x8001;
		uint64_t free = llrand() % 0x8001;
		uint64_t offset = llrand();
		uint64_t rwx = llrand();
		uint64_t lock = llrand();
		cap = CAP_MEMORY(begin, end, offset, rwx);
		cap.memory.lock = lock;
		cap.memory.free = free;
		s3k_cap = s3k_memory(begin, end, offset, rwx);
		s3k_cap.memory.lock = lock;
		s3k_cap.memory.free = free;
		ASSERT_EQ(cap.raw, s3k_cap.raw);
	}
}

TEST_F(ApiTest, MemoryParent)
{
	// Test if memory capabilities match
	union cap cap, cap_child;
	union s3k_cap s3k_cap, s3k_cap_child;
	// Test if parent predicate match
	int children = 0;
	while (children < 120) {
		uint64_t raw1 = llrand();
		uint64_t raw2 = llrand();
		cap.raw = raw1;
		cap_child.raw = raw2;
		s3k_cap.raw = raw1;
		s3k_cap_child.raw = raw2;
		ASSERT_EQ(cap_memory_parent(cap, cap_child),
			  s3k_memory_parent(s3k_cap, s3k_cap_child));
		if (cap_memory_parent(cap, cap_child))
			children++;
	}
}
