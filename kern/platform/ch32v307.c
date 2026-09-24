#include "csr.h"
#include "ipc.h"
#include "lock.h"
#include "mem.h"
#include "mon.h"
#include "pmp.h"
#include "proc.h"
#include "sched.h"
#include "syscall.h"
#include "tsl.h"
#include "usbfs_cdc.h" /* USBFS CDC debug console */

/* CH32V307 platform layer.
 *
 * Memory map: SRAM 0x20000000 (64KB), flash 0x08000000 (256KB), USART1
 * 0x40013800 (console). All partitions are loaded into SRAM so the
 * capability system can hand out RWX windows, mirroring the QEMU layout.
 */

/* Partition memory map (power-of-two NAPOT regions):
 *   app1 (PID1 monitor)   0x20000000 +16K
 *   app3 (PID3 comm)      0x20004000 +16K
 *   app2 (PID2 attitude)  0x20008000 +32K
 *   kernel (M-mode)       0x2000C800 +14K (no PMP check on M-mode) */
#define RAM_PERM MEM_PERM_RWX
#define RAM_BASE 0x20000000
#define RAM_SIZE 0x00010000

#define BOOT_RAM_BASE 0x20000000
#define BOOT_RAM_SIZE 0x00004000 /* PID1's partition footprint (16K). Its PMP
				    grant is the full 64K root window — see
				    kernel_init. */

/* app1 (PID1 monitor) code image in flash. */
#define APP1_FLASH_BASE 0x08018000u
#define APP1_FLASH_SIZE 0x00010000u /* 64K NAPOT cover for the code image. */

#define UART_PERM MEM_PERM_RW
#define UART_BASE 0x40013800 /* USART1 (0x40004400 is USART2!) */
#define UART_SIZE 0x20

/* The RTC backend polls the STK/SysTick core timer (read-only low word of
 * CNT at 0xE000F008, HCLK/8 = 18 MHz at the 144MHz PLL regime) against a
 * software shadow; _RTC_HZ is 18000000. */

void ch32v307_stk_init(void);

/* ---- USART1 console init ----
 * Without it the UART peripheral stays disabled and every userspace puts()
 * spins forever on TXE. */
#define RCC_BASE 0x40021000u
#define RCC_CFGR (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_APB2PCENR (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define GPIOA_BASE 0x40010800u
#define GPIOA_CFGHR (*(volatile uint32_t *)(GPIOA_BASE + 0x04))
#define USART1_BASE_HW 0x40013800u
#define USART_STATR (*(volatile uint32_t *)(USART1_BASE_HW + 0x00))
#define USART_BRR (*(volatile uint32_t *)(USART1_BASE_HW + 0x08))
#define USART_CTLR1 (*(volatile uint32_t *)(USART1_BASE_HW + 0x0C))

static void ch32v307_uart_init(void)
{
	/* Enable GPIOA(bit2) + USART1(bit14) clocks. */
	RCC_APB2PCENR |= (1u << 2) | (1u << 14);
	(void)RCC_APB2PCENR; /* read-back so the bus clock takes effect */
	for (volatile int i = 0; i < 8; i++)
		__asm__ volatile("nop");

	/* PA9 = AF push-pull 50MHz (0xB), PA10 = floating input (0x4);
	 * pins 8..15 live in CFGHR (offset 0x04), CFGLR only covers PA0..PA7. */
	GPIOA_CFGHR = (GPIOA_CFGHR & ~0x000FF000u) | (0xBu << 4) | (0x4u << 8);

	/* The flash loader auto-resets the chip, so the factory flash program
	 * may have already switched SYSCLK to the 144MHz PLL before our entry:
	 * read the actual clock source and pick the baud divider accordingly. */
	uint32_t sws = (RCC_CFGR >> 2) & 3u; /* 0=HSI 1=HSE 2=PLL 3=HSI48 */
	uint32_t pclk2 = (sws == 2u) ? 144000000u : 8000000u;
	USART_BRR = pclk2 / 115200u;
	USART_CTLR1 = (1u << 13) | (1u << 3) | (1u << 2); /* UE + TE + RE */
}

/* ---- QingKe V4F PMP isolation semantics (measured on hardware) ----
 *   - U-mode accesses matching NO entry are ALLOWED (the priv-spec-1.12
 *     "no match => fail" rule is NOT implemented), even with zero
 *     entries configured;
 *   - pmpcfg1 (entries 4-7) writes are silently ignored (reads back 0);
 *   - an explicit low-priority deny-all entry in a slot 0-3 (NAPOT,
 *     pmpaddr = 0xFFFFFFFF) traps every remaining U-mode access:
 *     instruction fetch -> cause 1, store -> cause 7.
 * So each partition keeps its grants in slots 0-2 and the kernel arms a
 * deny-all in slot 3; without it the demo attacker write to PID2's gain
 * region simply lands. */
void platform_proc_pmp_deny(pid_t pid)
{
	proc_pmp_set((pid_t)pid, (pmp_slot_t)3, MEM_PERM_NONE, 0xFFFFFFFF);
}

void kernel_init(void)
{
	ch32v307_uart_init();
	ch32v307_stk_init();
	usbfs_cdc_init(); /* USBFS CDC console: clock + SIE + PFIC arm */
	mem_t init_mem[NUM_MEMORY_CAPS] = {
		{.rwx = RAM_PERM,  .base = RAM_BASE,  .size = RAM_SIZE },
		{.rwx = UART_PERM, .base = UART_BASE, .size = UART_SIZE},
	};

	mem_init(init_mem);
	tsl_init();
	mon_init();
	ipc_init();
	sched_init();
	lock_init();
	proc_init(APP1_FLASH_BASE); /* PID1 entry: app1 _start in flash. */

	/* PID1 grant layout (mem_pmp_set maps mem-slot N -> PCB slot N-1):
	 *   PCB 0 (mem-slot 1): SRAM 0x20000000 +64K RWX (root RAM cap, as on
	 *              qemu_virt whose BOOT_RAM_SIZE spans the whole guest RAM)
	 *   PCB 1 (mem-slot 2): UART console RW — claimed by app1's setup_uart()
	 *   PCB 2:              flash 0x08000000 +256K RX (the 64K request
	 *              rounds up to the whole flash because 0x08018000 is not
	 *              64K-aligned)
	 *   PCB 3:              deny-all fallback (platform_proc_pmp_deny) */
	mem_pmp_set((pid_t)1, (index_t)0, (pmp_slot_t)1, RAM_PERM,
		    pmp_napot_encode(RAM_BASE, RAM_SIZE));
	proc_pmp_set((pid_t)1, (pmp_slot_t)2, MEM_PERM_RX,
		     pmp_napot_encode(APP1_FLASH_BASE, APP1_FLASH_SIZE));
	platform_proc_pmp_deny(1);

	/* Finish USB enumeration before the first partition prints (cable
	 * attached: ~100 ms; no cable: fixed ~2 s tick overhead). Bounded
	 * poll - MIE is still off here, so the device is driven straight out
	 * of INT_FG instead of IRQs. */
	usbfs_cdc_enum_wait();
	/* (kernel-stack A5 watermark lives in head.S, before any C frame
	 * exists — a C-level fill would overwrite its own return address.) */
}

void temporal_fence(void)
{
}

/* ---- SysTick-backed RTC (implements rtc.h on CH32V307) ----
 * The STK/SysTick core timer is the only reliable time source on this
 * board: TIM1's PSC prescaler never divides unless CTLR1 bit 13 is set
 * (undocumented for silicon revisions 4-8), TIM2 register writes are
 * ignored, and there is no CLINT/mtime. Layout (core manual): CTLR@0xE000F000,
 * SR@+0x04, CNT(64)@+0x08, CMP(64)@+0x10. The DS clock tree clocks the core
 * system timer at HCLK/8 = 18MHz in the 144MHz PLL regime. The counter is an
 * UP counter; only the 32-bit LOW word is polled, through unsigned delta
 * arithmetic; the upper word misbehaves under debugger halts and is not
 * read. Wrap/reload jumps (huge deltas) are skipped; a 2^32 wrap at 18MHz
 * takes 238s, and each wrap skips a single poll's worth of time -
 * invisible to the scheduler. mip.MTIP never asserts (PFIC, no CLINT) and
 * COUNTIF cannot be cleared, so this backend uses NO interrupts and NO
 * flags: pure read-only polling of CNTL against a software shadow.
 */
#include "rtc.h"

/* Scratch lo/hi pairs backing rtc.c's generic rv32 rtc_get/set_time paths
 * (see kern/src/rtc.c: extern __mtime[2] / __mtimecmp[hart][2]). Real
 * .bss storage defined here — linker-symbol aliases would let the
 * un-captured .sbss orphan section land on top of them. */
volatile uint32_t __mtime[2];
volatile uint32_t __mtimecmp[1][2]; /* nharts == 1 */

#define STK_BASE 0xE000F000u
#define STK_CTLR (*(volatile uint32_t *)(STK_BASE + 0x00))
#define STK_CNTL (*(volatile uint32_t *)(STK_BASE + 0x08))
/* STK_CNTH (+0x0C) deliberately NOT read: inconsistent under WCH-Link
 * halts. STK_CMP (+0x10) deliberately NOT touched: re-writing it puts
 * the counter into undocumented reload behaviour. The boot ROM leaves
 * the timer enabled, so the only write is a defensive one to set STE. */

/* Software monotonic clock shadow (STK ticks @HCLK/8 = 18MHz at PLL). */
static volatile uint64_t stk_shadow;
static uint32_t stk_last;
static bool stk_last_set;
volatile uint32_t tim_probe; /* 32-bit probe of the shadow clock (debug) */
volatile uint32_t tim_polls; /* debug: stk_poll() call count */
volatile uint32_t tim_valid; /* debug: accepted (nonzero, in-range) deltas */
volatile uint32_t tim_cnt;   /* debug: raw STK_CNTL at last poll */

void ch32v307_stk_init(void)
{
	stk_last = 0;
	stk_last_set = false;

	/* Point at the low word of the CNT without touching CMP/SR. */
	uint32_t ctl = STK_CTLR;
	if (!(ctl & 1u))
		STK_CTLR = ctl | 1u; /* STE: enable (STCLK=0 -> HCLK/8) */

	stk_last = STK_CNTL;
	stk_last_set = true;
	/* stk_shadow adopt 0 via rtc_set_time(0) at sched_init; nothing else
	 * needed - the delta math is immune to the raw counter's value. */
}

/* Advance the software clock by the counter delta since the last poll.
 * The STK CNT is an up counter, so delta = now - last. The 2^32 wrap edge
 * (now small, last near 2^32) looks like a huge jump and is skipped by
 * the <2^31 filter, as is a comparator-stalled counter; worst case the
 * shadow loses one poll's worth of advance - irrelevant at 1ms scheduler
 * granularity. */
static uint64_t stk_poll(void)
{
	uint32_t now = STK_CNTL;
	tim_polls++;
	tim_cnt = now;
	if (stk_last_set) {
		uint32_t delta = now - stk_last; /* up counter */
		uint32_t limit = 1u << 31;
		if (delta != 0u && delta < limit) {
			stk_shadow += delta;
			tim_valid++;
		}
	}
	stk_last = now;
	stk_last_set = true;
	tim_probe = (uint32_t)(stk_shadow / 64u); /* 281K/s at 18MHz */
	return stk_shadow;
}

uint64_t rtc_get_time(void)
{
	return stk_poll();
}

void rtc_set_time(uint64_t time)
{
	/* The software shadow is the clock: just adopt the requested value
	 * (sched_init calls this with 0 at boot). */
	stk_shadow = time;
}

uint64_t rtc_get_timeout(word_t hartid)
{
	uint32_t lo = __mtimecmp[hartid][0];
	uint32_t hi = __mtimecmp[hartid][1];
	return ((uint64_t)hi << 32) | lo;
}

void rtc_set_timeout(word_t hartid, uint64_t time)
{
	__mtimecmp[hartid][0] = (uint32_t)time;
	__mtimecmp[hartid][1] = (uint32_t)(time >> 32);
	/* Poll so the timeout is evaluated against a fresh clock. */
	stk_poll();
}

/* Preemption source: true iff the software shadow clock has reached the
 * armed timeout (see preempt.h). No hardware interrupt flag is usable:
 * QingKe V4F has no MTIP and the SysTick COUNTIF cannot be cleared, so
 * preemption is purely clock-poll driven. */
bool platform_timer_pending(void)
{
	return rtc_get_time() >= rtc_get_timeout(0);
}
