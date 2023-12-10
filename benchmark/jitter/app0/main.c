#include "altc/altio.h"
#include "s3k/s3k.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

#define APP0_PID 0
#define APP1_PID 1
#define APP2_PID 2
#define APP3_PID 3

// See plat_conf.h
#define BOOT_PMP 0
#define RAM_MEM 1
#define UART_MEM 2
#define TIME_MEM 3
#define HART1_TIME 4
#define MONITOR 8
#define CHANNEL 9
#define MONITOR_APP1 10
#define MONITOR_APP2 11
#define MONITOR_APP3 12

#define ASSERT(x)                                                      \
	{                                                              \
		s3k_err_t err = (x);                                   \
		if (err) {                                             \
			alt_printf("Failed at %s: err=%d\n", #x, err); \
			while (1)                                      \
				;                                      \
		}                                                      \
	}

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

void random_cidx(s3k_cidx_t *idx, int cnt)
{
	for (int i = 0; i < cnt; ++i) {
	retry:
		idx[i] = random() % S3K_CAP_CNT;
		for (int j = 0; j < i; ++j) {
			if (idx[j] == idx[i])
				goto retry;
		}
	}
}

void flush_data_cache()
{
	// 32 KiB 8-way cache.
	for (int i = 0; i < L1_TOTAL_SIZE; i += L1_LINE_SIZE) {
		data[i / sizeof(uint64_t)] = random();
	}
}

void setup_uart(uint64_t uart_idx)
{
	uint64_t uart_addr = s3k_napot_encode(UART0_BASE_ADDR, 0x8);
	// Derive a PMP capability for accessing UART
	s3k_cap_derive(UART_MEM, uart_idx, s3k_mk_pmp(uart_addr, S3K_MEM_RW));
	// Load the derive PMP capability to PMP configuration
	s3k_pmp_load(uart_idx, 1);
	// Synchronize PMP unit (hardware) with PMP configuration
	// false => not full synchronization.
	s3k_sync_mem();
}

void teardown(void)
{
	ASSERT(s3k_mon_suspend(MONITOR_APP1, APP1_PID));
	ASSERT(s3k_cap_revoke(MONITOR_APP2));
	ASSERT(s3k_mon_suspend(MONITOR_APP2, APP2_PID));
	ASSERT(s3k_mon_suspend(MONITOR_APP3, APP3_PID));
	ASSERT(s3k_cap_revoke(RAM_MEM));
	ASSERT(s3k_cap_revoke(HART1_TIME));
	ASSERT(s3k_cap_revoke(CHANNEL));

	s3k_state_t state;
	do {
		s3k_mon_state_get(MONITOR_APP1, APP1_PID, &state);
	} while (state != S3K_PSF_SUSPENDED);
	do {
		s3k_mon_state_get(MONITOR_APP2, APP2_PID, &state);
	} while (state != S3K_PSF_SUSPENDED);
	do {
		s3k_mon_state_get(MONITOR_APP3, APP3_PID, &state);
	} while (state != S3K_PSF_SUSPENDED);

	for (int i = 0; i < S3K_REG_CNT; ++i) {
		s3k_mon_reg_write(MONITOR_APP1, APP1_PID, i, 0);
		s3k_mon_reg_write(MONITOR_APP2, APP2_PID, i, 0);
	}
}

void setup_active(void)
{
	int tmp = 16;
	s3k_cidx_t indices[8];
	random_cidx(indices, ARRAY_SIZE(indices));

	uint64_t app1_begin = 0x80020000;
	uint64_t app1_end = 0x80040000;
	uint64_t app2_begin = 0x80040000;
	uint64_t app2_end = 0x80060000;

	s3k_cap_t ram1_mem = s3k_mk_memory(app1_begin, app1_end, S3K_MEM_RWX);
	s3k_cap_t ram1_pmp = s3k_mk_pmp(
	    s3k_napot_encode(app1_begin, app1_end - app1_begin), S3K_MEM_RWX);
	s3k_cap_t ram2_mem = s3k_mk_memory(app2_begin, app2_end, S3K_MEM_RWX);
	s3k_cap_t ram2_pmp = s3k_mk_pmp(
	    s3k_napot_encode(app2_begin, app2_end - app2_begin), S3K_MEM_RWX);
	s3k_cap_t ramX_mem = s3k_mk_memory(app1_end, app2_end, S3K_MEM_RWX);

	ASSERT(s3k_cap_derive(RAM_MEM, tmp, ram1_mem));
	ASSERT(s3k_cap_derive(tmp, tmp + 1, ram1_pmp));
	ASSERT(s3k_cap_derive(RAM_MEM, tmp + 2, ramX_mem));
	ASSERT(s3k_cap_derive(tmp + 2, tmp + 3, ram2_mem));
	ASSERT(s3k_cap_derive(tmp + 3, tmp + 4, ram2_pmp));

	ASSERT(s3k_mon_cap_move(MONITOR_APP1, APP0_PID, tmp, APP1_PID,
				indices[0]));
	ASSERT(s3k_mon_cap_move(MONITOR_APP1, APP0_PID, tmp + 1, APP1_PID,
				indices[1]));
	ASSERT(s3k_mon_cap_move(MONITOR_APP1, APP0_PID, tmp + 2, APP1_PID,
				indices[2]));
	ASSERT(s3k_mon_pmp_load(MONITOR_APP1, APP1_PID, indices[1], 0));

	ASSERT(s3k_mon_cap_move(MONITOR_APP2, APP0_PID, tmp + 4, APP2_PID, 0));
	ASSERT(s3k_mon_cap_move(MONITOR_APP2, APP0_PID, tmp + 3, APP2_PID, 1));
	ASSERT(s3k_mon_pmp_load(MONITOR_APP2, APP2_PID, 0, 0));

	s3k_cap_t hart0_time1 = s3k_mk_time(1, 0, 20);
	s3k_cap_t hart0_time2 = s3k_mk_time(1, 0, 4);

	ASSERT(s3k_cap_derive(HART1_TIME, tmp, hart0_time1));
	ASSERT(s3k_cap_derive(tmp, tmp + 1, hart0_time2));
	ASSERT(s3k_mon_cap_move(MONITOR_APP1, APP0_PID, tmp, APP1_PID,
				indices[3]));
	ASSERT(s3k_mon_cap_move(MONITOR_APP2, APP0_PID, tmp + 1, APP2_PID, 2));

	s3k_cap_t chan_app1 = s3k_mk_channel(0, S3K_CHAN_CNT);
	s3k_cap_t sock_app2 = s3k_mk_socket(0, S3K_IPC_YIELD, 0xF, 0);
	s3k_cap_t sock_app1 = s3k_mk_socket(0, S3K_IPC_YIELD, 0xF, 2);
	ASSERT(s3k_cap_derive(CHANNEL, tmp, chan_app1));
	ASSERT(s3k_cap_derive(tmp, tmp + 1, sock_app2));
	ASSERT(s3k_cap_derive(tmp + 1, tmp + 2, sock_app1));
	ASSERT(s3k_mon_cap_move(MONITOR_APP1, APP0_PID, tmp, APP1_PID,
				indices[4]));
	ASSERT(s3k_mon_cap_move(MONITOR_APP2, APP0_PID, tmp + 1, APP2_PID, 3));
	ASSERT(s3k_mon_cap_move(MONITOR_APP1, APP0_PID, tmp + 2, APP1_PID,
				indices[5]));

	s3k_mon_reg_write(MONITOR_APP2, APP2_PID, S3K_REG_PC, app2_begin);

	s3k_cap_t mon_app2 = s3k_mk_monitor(2, 3);
	ASSERT(s3k_cap_derive(MONITOR_APP2, tmp, mon_app2));
	ASSERT(s3k_mon_cap_move(MONITOR_APP1, APP0_PID, tmp, APP1_PID,
				indices[6]));

	s3k_mon_reg_write(MONITOR_APP1, APP1_PID, S3K_REG_PC, app1_begin);
}

void setup_flush(void)
{
	uint64_t app3_base = 0x80060000;
	s3k_cap_t hart0_time = s3k_mk_time(1, 20, 28);
	s3k_cap_t ram3_pmp
	    = s3k_mk_pmp(s3k_napot_encode(app3_base, 0x20000), S3K_MEM_RWX);
	ASSERT(s3k_cap_derive(HART1_TIME, 16, hart0_time));
	ASSERT(s3k_mon_cap_move(MONITOR_APP3, APP0_PID, 16, APP3_PID, 1));
	ASSERT(s3k_cap_derive(RAM_MEM, 16, ram3_pmp));
	ASSERT(s3k_mon_cap_move(MONITOR_APP3, APP0_PID, 16, APP3_PID, 0));
	ASSERT(s3k_mon_pmp_load(MONITOR_APP3, APP3_PID, 0, 0));
	ASSERT(
	    s3k_mon_reg_write(MONITOR_APP3, APP3_PID, S3K_REG_PC, app3_base));
}

uint64_t csrr_cycle(void)
{
	register uint64_t cycle;
	__asm__ volatile("csrr %0, cycle" : "=r"(cycle));
	return cycle;
}

#define MEASUREMENTS 10000

void measurement(void)
{
	uint64_t start, end;
	s3k_sleep(0);
	for (int i = 0; i < MEASUREMENTS; ++i) {
	retry:
		teardown();
		setup_active();
		setup_flush();
		flush_data_cache();
		s3k_sleep(0);
		start = csrr_cycle();
#if defined(SCENARIO_SOLO)
		/* Nothing */
#elif defined(SCENARIO_ACTIVE)
		s3k_mon_resume(MONITOR_APP1, APP1_PID);
#elif defined(SCENARIO_FLUSH)
		s3k_mon_resume(MONITOR_APP3, APP3_PID);
#elif defined(SCENARIO_ACTIVE_FLUSH)
		s3k_mon_resume(MONITOR_APP3, APP3_PID);
		s3k_mon_resume(MONITOR_APP1, APP1_PID);
#endif
		s3k_sleep(0);
		end = csrr_cycle();
		alt_printf("%d\t%D\n", i + 1, end - start);
	}
}

int main(void)
{
	// Setup UART access
	setup_uart(6);
	s3k_cap_t mon = s3k_mk_monitor(1, 3);
	ASSERT(s3k_cap_derive(MONITOR, MONITOR_APP1, s3k_mk_monitor(1, 2)));
	ASSERT(s3k_cap_derive(MONITOR, MONITOR_APP2, s3k_mk_monitor(2, 3)));
	ASSERT(s3k_cap_derive(MONITOR, MONITOR_APP3, s3k_mk_monitor(3, 4)));

	measurement();
}
