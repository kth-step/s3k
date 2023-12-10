#include "altc/altio.h"
#include "s3k/s3k.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define L1_TOTAL_SIZE (32 * 1024)
#define L1_LINE_SIZE 64

static volatile uint64_t data[L1_TOTAL_SIZE / sizeof(uint64_t)];

uint64_t random(void)
{
	static unsigned long x = 123456789, y = 362436069, z = 521288629;
	unsigned long t;
	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;

	return z;
}

uint64_t randint(unsigned long min, unsigned long max)
{
	return (random() % (max - min)) + min;
}

void flush_data_cache()
{
	// 32 KiB 8-way cache.
	for (int i = 0; i < L1_TOTAL_SIZE; i += L1_LINE_SIZE) {
		data[i / sizeof(uint64_t)] = random();
	}
}

void random_data_access(int x)
{
	for (int i = 0; i < x; ++i) {
		data[random() % ARRAY_SIZE(data)] = random();
	}
}

// Known capabilities.
s3k_cap_t caps[S3K_CAP_CNT];

void refresh_caps(bool all)
{
	for (int i = 0; i < S3K_CAP_CNT; ++i) {
		if (all || caps[i].type != 0)
			s3k_cap_read(i, caps + i);
	}
}

void test_get_info(void)
{
	switch (random() % 3) {
	case 0:
		s3k_get_pid();
	case 1:
		s3k_get_time();
	case 2:
		s3k_get_timeout();
	}
}

void test_reg_read(void)
{
	s3k_reg_read(random() % S3K_REG_CNT);
}

void test_reg_write(void)
{
	s3k_reg_write(random() % S3K_REG_CNT, random());
}

void test_sync()
{
	if (random() % 2) {
		s3k_sync();
	} else {
		s3k_sync_mem();
	}
}

void test_cap_move(void)
{
	int i, j;
retry:
	i = random() % S3K_CAP_CNT;
	j = random() % S3K_CAP_CNT;
	if (caps[i].type == S3K_CAPTY_NONE || caps[j].type != S3K_CAPTY_NONE)
		goto retry;
	s3k_cap_move(i, j);
	caps[i].raw = 0;
	caps[j].raw = caps[j].raw;
}

void test_cap_delete()
{
	int i;
retry:
	i = random() % S3K_CAP_CNT;
	s3k_cap_delete(i);
	s3k_cap_read(i, caps + i);
}

void test_cap_revoke()
{
	int i;
retry:
	i = random() % S3K_CAP_CNT;
	s3k_cap_revoke(i);
	refresh_caps(true);
}

void test_cap_derive_random(void)
{
	int i, j;
retry:
	i = random() % S3K_CAP_CNT;
	j = random() % S3K_CAP_CNT;
	if (caps[i].type == S3K_CAPTY_NONE || caps[j].type != S3K_CAPTY_NONE)
		goto retry;
	uint64_t raw = random() ^ (random() << 32);
	s3k_cap_derive(i, j, (s3k_cap_t){.raw = raw});
}

void test_cap_derive(void)
{
	int i, j;
retry:
	i = random() % S3K_CAP_CNT;
	j = random() % S3K_CAP_CNT;
	if (caps[i].type == S3K_CAPTY_NONE || caps[j].type != S3K_CAPTY_NONE)
		goto retry;
	s3k_cap_t new_cap;
	switch (caps[i].type) {
	case S3K_CAPTY_TIME: {
		new_cap.raw = caps[i].raw;
		uint64_t size = (new_cap.time.end - new_cap.time.mrk);
		new_cap.time.end -= random() % size;
	} break;
	case S3K_CAPTY_MEMORY: {
		new_cap.raw = caps[i].raw;
		uint64_t size = (new_cap.mem.end - new_cap.mem.mrk);
		new_cap.mem.end -= random() % size;
		new_cap.mem.rwx &= random();
	} break;
	case S3K_CAPTY_MONITOR: {
		new_cap.raw = caps[i].raw;
		uint64_t size = (new_cap.mon.end - new_cap.mon.mrk);
		new_cap.mon.end -= random() % size;
	} break;
	case S3K_CAPTY_CHANNEL: {
		new_cap.raw = caps[i].raw;
		uint64_t size = (new_cap.chan.end - new_cap.chan.mrk);
		new_cap.chan.end -= random() % size;
	} break;
	default:
		goto retry;
	}
	s3k_cap_derive(i, j, new_cap);
	s3k_cap_read(i, caps + i);
	s3k_cap_read(j, caps + j);
}

void test_pmp()
{
	int i, j;
retry:
	i = random() % S3K_CAP_CNT;
	if (caps[i].type != S3K_CAPTY_PMP)
		goto retry;
	if (caps[i].pmp.used) {
		s3k_pmp_unload(i);
	} else {
		s3k_pmp_load(i, random() % 8);
	}
	s3k_cap_read(i, caps + i);
}

void test_monitor()
{
	int i;
retry:
	i = randint(0, S3K_CAP_CNT);
	if (caps[i].type != S3K_CAPTY_MONITOR)
		goto retry;
	uint64_t min = caps[i].mon.mrk;
	uint64_t max = caps[i].mon.end;
	switch (random() % 7) {
	case 0:
		s3k_mon_suspend(i, randint(min, max));
		break;
	case 1:
		s3k_mon_resume(i, randint(min, max));
		break;
	case 2: {
		s3k_state_t state;
		s3k_mon_state_get(i, randint(min, max), &state);
	} break;
	case 3: {
		s3k_cap_t cap;
		s3k_mon_cap_read(i, randint(min, max), randint(0, S3K_CAP_CNT),
				 &cap);
	} break;
	case 4: {
		s3k_cap_t cap;
		s3k_mon_cap_move(i, 1, randint(0, S3K_CAP_CNT),
				 randint(min, max), randint(0, S3K_CAP_CNT));
	} break;
	case 5: {
		s3k_cap_t cap;
		s3k_mon_cap_move(i, randint(min, max), randint(0, S3K_CAP_CNT),
				 1, randint(0, S3K_CAP_CNT));
	} break;
	case 6: {
		s3k_cap_t cap;
		s3k_mon_cap_move(i, randint(min, max), randint(0, S3K_CAP_CNT),
				 randint(min, max), randint(0, S3K_CAP_CNT));
	} break;
	case 7: {
		s3k_cap_t cap;
		s3k_mon_cap_move(i, randint(min, max), randint(0, S3K_CAP_CNT),
				 randint(min, max), randint(0, S3K_CAP_CNT));
	} break;
	case 8: {
		s3k_cap_t cap;
		s3k_mon_pmp_load(i, randint(min, max), randint(0, S3K_CAP_CNT),
				 randint(0, 8));
	} break;
	case 9: {
		s3k_cap_t cap;
		s3k_mon_pmp_unload(i, randint(min, max),
				   randint(0, S3K_CAP_CNT));
	} break;
	}
}

void test_socket()
{
	int i, j;
retry:
	i = randint(0, S3K_CAP_CNT);
	j = randint(0, S3K_CAP_CNT);
	if (caps[i].type != S3K_CAPTY_SOCKET && caps[j].type != S3K_CAPTY_NONE)
		goto retry;
	s3k_msg_t msg = {.send_cap = 1, .cap_idx = j};
	switch (random() % 2) {
	case 0: {
		s3k_sock_send(i, &msg);
	} break;
	case 1: {
		s3k_sock_sendrecv(i, &msg);
	} break;
	}
}

typedef void (*tester_t)(void);

// clang-format: disable
tester_t testers[] = {
#if defined(SCENARIO_ALL)
    test_get_info,
    test_reg_read,
    test_reg_write,
    test_sync,
    test_cap_move,
    test_cap_delete,
    test_cap_revoke,
    test_cap_derive,
    test_cap_derive_random,
    test_pmp,
    test_monitor,
    test_socket
#elif defined(SCENARIO_CAP_OPS)
    test_cap_delete,
    test_cap_revoke,
    test_cap_derive,
    test_cap_derive_random,
#elif defined(SCENARIO_PMP)
    test_pmp, test_monitor, test_socket
#elif defined(SCENARIO_MONITOR)
    test_monitor,
#elif defined(SCENARIO_SOCKET)
    test_socket
#endif
};
// clang-format: enable

int main(void)
{
	refresh_caps(true);
	while (1) {
		if (random() % 2)
			flush_data_cache();
		else
			random_data_access(64);
		testers[random() % ARRAY_SIZE(testers)]();
	}
}
