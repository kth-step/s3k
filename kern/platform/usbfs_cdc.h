/* usbfs_cdc.h - CH32V307 USBFS CDC-ACM debug console (kernel side).
 *
 * The board's USART1 pins (PA9/PA10) have no electrically connected debug
 * channel (R20/R34 0-ohm optionals unpopulated), so the kernel runs a small
 * USBFS device stack instead: the chip's own USB connector turns into a CDC
 * ACM serial port on the PC (/dev/ttyACM*), and kernel console output is
 * drained onto EP3 (bulk IN).
 *
 * All state and code is kernel-side: M-mode ring, endpoint buffers and the
 * controller registers themselves. No physical wiring is needed.
 */
#pragma once

#include <stdint.h>

/* One-time init: USBFS clock (PLL/3 = 48 MHz @ the 144 MHz PLL regime),
 * SIE reset, endpoint setup, PFIC IRQ arm. No-op if SYSCLK is not the
 * 144 MHz PLL regime (USBFS needs an exact 48 MHz clock). */
void usbfs_cdc_init(void);

/* Bounded poll of the device controller: services control traffic directly
 * from INT_FG so enumeration completes even before the first mret (kernel
 * runs with MIE=0 until the first user-mode entry). Called once from
 * kernel_init; gives up after ~2 s if no host enumerates us (no cable). */
void usbfs_cdc_enum_wait(void);

/* USBFS interrupt routine (also the poll body). Reads INT_FG/INT_ST and
 * services TRANSFER / BUS_RST events: EP0 standard+CDC-class requests,
 * EP1/EP3 IN completions, EP2 OUT discard. Never blocks; never schedules. */
void usbfs_cdc_irq(void);

/* Console byte entry point. Pushes onto the TX ring and arms EP3 if idle.
 * Bytes are dropped (and counted) when the ring is full - this can only happen when no host is attached, since a
 * live CDC link drains the ring orders of magnitude faster than the
 * partitions print. */
void usbfs_cdc_putc(char c);

/* Debug/measurement stats (readable via a debugger at runtime). */
extern volatile uint32_t cdc_state;     /* 0=off/no-48M-clk 1=init 2=enumerating 3=configured */
extern volatile uint32_t cdc_irqs;      /* USBFS IRQ/poll service count */
extern volatile uint32_t cdc_txd;       /* console bytes actually sent on EP3 */
extern volatile uint32_t cdc_drops;     /* ring-overflow drops (host absent) */
extern volatile uint32_t cdc_rxd;       /* host->device bytes discarded (EP2) */
extern volatile uint32_t cdc_enum_ticks;/* STK ticks spent in enum wait */