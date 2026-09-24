/* usbfs_cdc.c - register-level USBFS (full-speed device) CDC-ACM console
 * for CH32V307VCT6, adapted from the official SimulateCDC example
 * (CH32V307EVT/EVT/EXAM/USB/USBFS/DEVICE/SimulateCDC) with all HAL and
 * UART2 dependencies removed, so it links into the freestanding S3K kernel.
 * Differences from the WCH example:
 *   - one-way console: EP3 (bulk IN) carries kernel-buffered console bytes,
 *     EP2 (bulk OUT) is ACK'd and discarded, EP1 (CDC interrupt IN) is armed
 *     but never used, SUSPEND interrupt is not enabled (no sleep support);
 *   - UEPn_DMA takes a 16-bit SRAM offset, so all endpoint DMA buffers and
 *     the TX ring live in the kernel RAM window inside the first 64 KB of
 *     SRAM;
 *   - the IRQ body runs from the S3K trap path (M-mode external interrupt
 *     11 via PFIC IRQ 83, kern/src/interrupt.c) and from the bounded
 *     boot-time poll (usbfs_cdc_enum_wait). It never blocks and never
 *     touches scheduler state, so PFIC IRQs can be serviced while a
 *     U-mode partition runs without rescheduling it.
 */

#include "usbfs_cdc.h"
#include <stdbool.h>
#include <stdint.h>

/* ---- QingKe V4F PFIC ---- */
#define PFIC_BASE 0xE000E000u
#define PFIC_IENR(n) (*(volatile uint32_t *)(PFIC_BASE + 4u * (n)))
#define USBFS_IRQn 83u

/* ---- RCC (offsets from ch32v30x RCC_TypeDef) ---- */
#define RCC_BASE 0x40021000u
#define RCC_CFGR0 (*(volatile uint32_t *)(RCC_BASE + 0x04u))
#define RCC_AHBPCENR (*(volatile uint32_t *)(RCC_BASE + 0x14u))
#define RCC_AHBPCENR_USBFS (1u << 12)
#define RCC_CFGR0_USBFSCLK_MASK (3u << 22)
#define RCC_CFGR0_USBFSCLK_PLLDIV3 (2u << 22)

/* ---- STK core timer (HCLK/8 = 18 MHz in the 144 MHz PLL regime) ---- */
#define STK_CNTL (*(volatile uint32_t *)(0xE000F008u))

/* ---- USBFS device registers (base 0x50000000, same file both views) ---- */
#define USBFS_BASE 0x50000000u
#define UD_BASE_CTRL (*(volatile uint8_t *)(USBFS_BASE + 0x00u))
#define UD_UDEV_CTRL (*(volatile uint8_t *)(USBFS_BASE + 0x01u))
#define UD_INT_EN (*(volatile uint8_t *)(USBFS_BASE + 0x02u))
#define UD_DEV_ADDR (*(volatile uint8_t *)(USBFS_BASE + 0x03u))
#define UD_MIS_ST (*(volatile uint8_t *)(USBFS_BASE + 0x05u))
#define UD_INT_FG (*(volatile uint8_t *)(USBFS_BASE + 0x06u))
#define UD_INT_ST (*(volatile uint8_t *)(USBFS_BASE + 0x07u))
#define UD_RX_LEN (*(volatile uint16_t *)(USBFS_BASE + 0x08u))
#define UD_UEP4_1_MOD (*(volatile uint8_t *)(USBFS_BASE + 0x0Cu))
#define UD_UEP2_3_MOD (*(volatile uint8_t *)(USBFS_BASE + 0x0Du))
#define UD_UEPn_DMA(n) (*(volatile uint32_t *)(USBFS_BASE + 0x10u + 4u * (n)))
#define UD_UEPn_TX_LEN(n) (*(volatile uint16_t *)(USBFS_BASE + 0x30u + 4u * (n)))
#define UD_UEPn_TX_CTRL(n) (*(volatile uint8_t *)(USBFS_BASE + 0x32u + 4u * (n)))
#define UD_UEPn_RX_CTRL(n) (*(volatile uint8_t *)(USBFS_BASE + 0x33u + 4u * (n)))

/* BASE_CTRL (device) */
#define UC_DEV_PU_EN 0x20u
#define UC_INT_BUSY 0x08u
#define UC_RESET_SIE 0x04u
#define UC_CLR_ALL 0x02u
#define UC_DMA_EN 0x01u
/* UDEV_CTRL */
#define UD_PD_DIS 0x80u
#define UD_PORT_EN 0x01u
/* INT_EN / INT_FG */
#define UIF_BUS_RST 0x01u
#define UIE_TRANSFER 0x02u
#define UIF_TRANSFER 0x02u
/* INT_ST token codes */
#define UIS_TOKEN_MASK 0x30u
#define UIS_TOKEN_OUT 0x00u
#define UIS_TOKEN_SOF 0x10u
#define UIS_TOKEN_IN 0x20u
#define UIS_TOKEN_SETUP 0x30u
#define UIS_ENDP_MASK 0x0Fu
/* endpoint enable bits */
#define UEP1_TX_EN 0x40u
#define UEP2_RX_EN 0x08u
#define UEP3_TX_EN 0x40u
/* endpoint response bits */
#define UEP_T_TOG 0x04u
#define UEP_R_TOG 0x04u
#define UEP_T_RES_MASK 0x03u
#define UEP_R_RES_MASK 0x03u
#define UEP_T_RES_ACK 0x00u
#define UEP_T_RES_NAK 0x02u
#define UEP_T_RES_STALL 0x03u
#define UEP_R_RES_ACK 0x00u
#define UEP_R_RES_NAK 0x02u
#define UEP_R_RES_STALL 0x03u
#define UDA_GP_BIT 0x80u

/* usb request codes / descriptor types (USB 2.0 ch.9) */
#define USB_REQ_TYP_IN 0x80u
#define USB_REQ_TYP_MASK 0x60u
#define USB_REQ_TYP_STANDARD 0x00u
#define USB_REQ_TYP_CLASS 0x20u
#define USB_REQ_TYP_VENDOR 0x40u
#define USB_GET_STATUS 0x00u
#define USB_CLEAR_FEATURE 0x01u
#define USB_SET_FEATURE 0x03u
#define USB_SET_ADDRESS 0x05u
#define USB_GET_DESCRIPTOR 0x06u
#define USB_GET_CONFIGURATION 0x08u
#define USB_SET_CONFIGURATION 0x09u
#define USB_GET_INTERFACE 0x0Au
#define USB_SET_INTERFACE 0x0Bu
#define USB_DESCR_TYP_DEVICE 0x01u
#define USB_DESCR_TYP_CONFIG 0x02u
#define USB_DESCR_TYP_STRING 0x03u
#define CDC_SET_LINE_CODING 0x20u
#define CDC_GET_LINE_CODING 0x21u
#define CDC_SET_LINE_CTLSTE 0x22u
#define CDC_SEND_BREAK 0x23u

#define EP0_SIZE 64u
#define EP1 1u
#define EP2 2u
#define EP3 3u

/* TX ring: lives in the kernel RAM window. 384 B keeps the extra .bss at
 * ~1 KB so the kernel stack (grows down from 0x20010000) keeps headroom;
 * a live CDC link drains it in microseconds, so it only overflows when no
 * host is attached, which cdc_drops then counts (see usbfs_cdc.h). */
#define CDC_RING_SIZE 384u

/* ---- USB descriptors (flash .rodata, copied from SimulateCDC layout) ---- */
static const uint8_t dev_descr[18] = {
	0x12, 0x01, 0x10, 0x01, 0x02, 0x00, 0x00, EP0_SIZE,
	0x86, 0x1A, /* idVendor 0x1A86 (WCH) */
	0x33, 0x53, /* idProduct 0x5333: 'S''3' - S3K CDC console */
	0x01, 0x00, 0x01, 0x02, 0x00, 0x01,
};

static const uint8_t cfg_descr[67] = {
	/* configuration */
	0x09, 0x02, 0x43, 0x00, 0x02, 0x01, 0x00, 0x80, 0x32,
	/* interface 0: CDC control (2/2/1) */
	0x09, 0x04, 0x00, 0x00, 0x01, 0x02, 0x02, 0x01, 0x00,
	/* functional: header, call-mgmt, ACM, union */
	0x05, 0x24, 0x00, 0x10, 0x01,
	0x05, 0x24, 0x01, 0x00, 0x01,
	0x04, 0x24, 0x02, 0x02,
	0x05, 0x24, 0x06, 0x00, 0x01,
	/* interrupt IN endpoint (never loaded, NAKed forever) */
	0x07, 0x05, 0x81, 0x03, EP0_SIZE, 0x00, 0x01,
	/* interface 1: CDC data (0x0A) */
	0x09, 0x04, 0x01, 0x00, 0x02, 0x0A, 0x00, 0x00, 0x00,
	/* bulk OUT endpoint (device-bound data, discarded) */
	0x07, 0x05, 0x02, 0x02, EP0_SIZE, 0x00, 0x00,
	/* bulk IN endpoint (console TX) */
	0x07, 0x05, 0x83, 0x02, EP0_SIZE, 0x00, 0x00,
};

static const uint8_t lang_descr[4] = { 0x04, 0x03, 0x09, 0x04 };
static const uint8_t manu_descr[14] = {
	0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0
};
static const uint8_t prod_descr[32] = {
	0x20, 0x03,
	'S', 0, '3', 0, 'K', 0, ' ', 0, 'C', 0, 'D', 0, 'C', 0, ' ', 0,
	'C', 0, 'o', 0, 'n', 0, 's', 0, 'o', 0, 'l', 0, 'e', 0
};
static const uint8_t sern_descr[18] = {
	0x12, 0x03,
	'2', 0, '0', 0, '2', 0, '6', 0, '0', 0, '8', 0, '2', 0, '1', 0
};

/* ---- runtime state (kernel .bss) ---- */
volatile uint32_t cdc_state;
volatile uint32_t cdc_irqs;
volatile uint32_t cdc_txd;
volatile uint32_t cdc_drops;
volatile uint32_t cdc_rxd;
volatile uint32_t cdc_enum_ticks;

/* Endpoint DMA buffers must sit in the first 64 KB of SRAM (UEPn_DMA is
 * the 16-bit SRAM offset). Kernel RAM window qualifies and is 4-byte
 * aligned by the attribute. EP0 gets 128 B: the SIE keeps the control RX
 * buffer at the DMA base and the TX staging at +64 in dual-buffer silicon
 * behaviour; provisioning both halves is safe under either behaviour. */
static __attribute__((aligned(4))) uint8_t ep0_buf[2 * EP0_SIZE];
static __attribute__((aligned(4))) uint8_t ep2_buf[EP0_SIZE]; /* host->dev discard */
static __attribute__((aligned(4))) uint8_t ep3_buf[EP0_SIZE]; /* console TX staging */

static uint8_t tx_ring[CDC_RING_SIZE];
static uint16_t tx_head, tx_tail; /* next push, next pop */
static uint8_t ep3_busy;
static uint8_t ep1_busy;

/* control-transfer pipeline (WCH example state) */
static volatile uint8_t setup_code, setup_type;
static volatile uint16_t setup_len, setup_value, setup_index;
static uint8_t dev_config, dev_addr;
static const uint8_t *descr_ptr;
static uint8_t line_coding[8] = { 0x00, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00 };
/* default 115200-8N1, timeout byte */

static bool clk_ok;

/* ---- tiny helpers (freestanding, no libc) ---- */
static void memcpy8(uint8_t *d, const uint8_t *s, uint16_t n)
{
	while (n--)
		*d++ = *s++;
}

static void delay_stk(uint32_t ticks) /* STK CNTL ticks @ 18 MHz */
{
	uint32_t start = STK_CNTL;
	while ((uint32_t)(STK_CNTL - start) < ticks) {
	}
}

/* ---- endpoint machinery ---- */
static void ep_init(void)
{
	UD_UEP4_1_MOD = UEP1_TX_EN;
	UD_UEP2_3_MOD = UEP2_RX_EN | UEP3_TX_EN;

	UD_UEPn_DMA(0) = (uint32_t)(uintptr_t)ep0_buf;
	UD_UEPn_DMA(EP1) = (uint32_t)(uintptr_t)ep3_buf; /* never armed/accessed */
	UD_UEPn_DMA(EP2) = (uint32_t)(uintptr_t)ep2_buf;
	UD_UEPn_DMA(EP3) = (uint32_t)(uintptr_t)ep3_buf;

	UD_UEPn_RX_CTRL(0) = UEP_R_RES_ACK;
	UD_UEPn_RX_CTRL(EP2) = UEP_R_RES_ACK;

	UD_UEPn_TX_LEN(EP1) = 0;
	UD_UEPn_TX_LEN(EP3) = 0;

	UD_UEPn_TX_CTRL(0) = UEP_T_RES_NAK;
	UD_UEPn_TX_CTRL(EP1) = UEP_T_RES_NAK;
	UD_UEPn_TX_CTRL(EP3) = UEP_T_RES_NAK;

	ep1_busy = 0;
	ep3_busy = 0;
}

/* Arm the next console packet on EP3 if possible. */
static void ep3_kick(void)
{
	uint16_t n;
	if (!dev_config || ep3_busy)
		return;
	if (tx_head == tx_tail)
		return;

	n = 0;
	while (n < EP0_SIZE && tx_head != tx_tail) {
		ep3_buf[n++] = tx_ring[tx_tail++];
		if (tx_tail == CDC_RING_SIZE)
			tx_tail = 0;
	}
	UD_UEPn_TX_LEN(EP3) = n;
	UD_UEPn_TX_CTRL(EP3) ^= UEP_T_TOG;
	UD_UEPn_TX_CTRL(EP3) = (UD_UEPn_TX_CTRL(EP3) & ~UEP_T_RES_MASK) |
			       UEP_T_RES_ACK;
	ep3_busy = 1;
}

static const uint8_t *get_descr(uint8_t type, uint8_t idx, uint16_t *len)
{
	switch (type) {
	case USB_DESCR_TYP_DEVICE:
		*len = sizeof(dev_descr);
		return dev_descr;
	case USB_DESCR_TYP_CONFIG:
		*len = sizeof(cfg_descr);
		return cfg_descr;
	case USB_DESCR_TYP_STRING:
		switch (idx) {
		case 0:
			*len = sizeof(lang_descr);
			return lang_descr;
		case 1:
			*len = sizeof(manu_descr);
			return manu_descr;
		case 2:
			*len = sizeof(prod_descr);
			return prod_descr;
		case 3:
			*len = sizeof(sern_descr);
			return sern_descr;
		default:
			return 0;
		}
	default:
		return 0;
	}
}

/* ---- the IRQ body (also used as the boot-time poll body) ---- */
void usbfs_cdc_irq(void)
{
	uint8_t intflag = UD_INT_FG;
	uint8_t intst = UD_INT_ST;
	uint16_t len;
	uint8_t errflag = 0;

	cdc_irqs++;

	if (intflag & UIF_TRANSFER) {
		switch (intst & UIS_TOKEN_MASK) {
		case UIS_TOKEN_IN:
			switch (intst & (UIS_TOKEN_MASK | UIS_ENDP_MASK)) {
			case UIS_TOKEN_IN | 0: /* EP0 data-in */
				if (setup_len == 0)
					UD_UEPn_RX_CTRL(0) = UEP_R_TOG | UEP_R_RES_ACK;
				if ((setup_type & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) {
					/* class EP0 IN: nothing further (single chunk) */
				} else {
					switch (setup_code) {
					case USB_GET_DESCRIPTOR:
						len = setup_len >= EP0_SIZE ? EP0_SIZE : setup_len;
						memcpy8(ep0_buf, descr_ptr, len);
						setup_len -= len;
						descr_ptr += len;
						UD_UEPn_TX_LEN(0) = len;
						UD_UEPn_TX_CTRL(0) ^= UEP_T_TOG;
						break;
					case USB_SET_ADDRESS:
						UD_DEV_ADDR = (UD_DEV_ADDR & UDA_GP_BIT) | dev_addr;
						break;
					default:
						break;
					}
				}
				break;

			case UIS_TOKEN_IN | EP1:
				UD_UEPn_TX_CTRL(EP1) ^= UEP_T_TOG;
				UD_UEPn_TX_CTRL(EP1) = (UD_UEPn_TX_CTRL(EP1) & ~UEP_T_RES_MASK) |
						       UEP_T_RES_NAK;
				ep1_busy = 0;
				break;

			case UIS_TOKEN_IN | EP3:
				UD_UEPn_TX_CTRL(EP3) ^= UEP_T_TOG;
				UD_UEPn_TX_CTRL(EP3) = (UD_UEPn_TX_CTRL(EP3) & ~UEP_T_RES_MASK) |
						       UEP_T_RES_NAK;
				ep3_busy = 0;
				ep3_kick(); /* drain the ring, send the next chunk */
				break;

			default:
				break;
			}
			break;

		case UIS_TOKEN_OUT:
			switch (intst & (UIS_TOKEN_MASK | UIS_ENDP_MASK)) {
			case UIS_TOKEN_OUT | 0: { /* EP0 data-out */
				len = UD_RX_LEN;
				if (setup_type & USB_REQ_TYP_IN) {
					/* we ACKed nothing for IN: ignore */
				} else {
					if ((setup_type & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) {
						setup_len = 0;
						if (setup_code == CDC_SET_LINE_CODING && len >= 7) {
							for (int i = 0; i < 7; i++)
								line_coding[i] = ep0_buf[i];
							line_coding[7] = 0;
						}
					} else {
						/* no standard EP0 OUT data in this device */
					}
					if (setup_len == 0) {
						UD_UEPn_TX_LEN(0) = 0;
						UD_UEPn_TX_CTRL(0) = UEP_T_TOG | UEP_T_RES_ACK;
					}
				}
				break;
			}

			case UIS_TOKEN_OUT | EP2:
				/* host->device byte stream: discard (console is
				 * one-way), keep EP2 ACKing. */
				cdc_rxd += UD_RX_LEN;
				UD_UEPn_RX_CTRL(EP2) ^= UEP_R_TOG;
				break;

			default:
				break;
			}
			break;

		case UIS_TOKEN_SETUP: {
			UD_UEPn_TX_CTRL(0) = UEP_T_TOG | UEP_T_RES_NAK;
			UD_UEPn_RX_CTRL(0) = UEP_R_TOG | UEP_R_RES_NAK;

			setup_type = ep0_buf[0];
			setup_code = ep0_buf[1];
			setup_value = (uint16_t)(ep0_buf[2] | (ep0_buf[3] << 8));
			setup_index = (uint16_t)(ep0_buf[4] | (ep0_buf[5] << 8));
			setup_len = (uint16_t)(ep0_buf[6] | (ep0_buf[7] << 8));
			len = 0;
			errflag = 0;

			if ((setup_type & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) {
				if (setup_type & USB_REQ_TYP_CLASS) {
					switch (setup_code) {
					case CDC_GET_LINE_CODING:
						descr_ptr = &line_coding[0];
						len = 7;
						break;
					case CDC_SET_LINE_CODING:
					case CDC_SET_LINE_CTLSTE:
					case CDC_SEND_BREAK:
						descr_ptr = 0; /* data stage handled in OUT */
						break;
					default:
						descr_ptr = 0;
						errflag = 0xff;
						break;
					}
				} else {
					descr_ptr = 0;
					errflag = 0xff; /* vendor requests unsupported */
				}
				/* Stage IN data only when a descriptor was selected;
				 * otherwise the SIE will overwrite ep0_buf with the OUT
				 * data stage (e.g. SET_LINE_CODING's 7 bytes). */
				if (descr_ptr) {
					len = setup_len >= EP0_SIZE ? EP0_SIZE : setup_len;
					memcpy8(ep0_buf, descr_ptr, len);
					descr_ptr += len;
				}
			} else {
				switch (setup_code) {
				case USB_GET_DESCRIPTOR: {
					const uint8_t *p;
					uint16_t dlen;
					p = get_descr((uint8_t)(setup_value >> 8),
						      (uint8_t)(setup_value & 0xFF), &dlen);
					if (!p) {
						errflag = 0xff;
						break;
					}
					descr_ptr = p;
					if (setup_len > dlen)
						setup_len = dlen;
					len = setup_len >= EP0_SIZE ? EP0_SIZE : setup_len;
					memcpy8(ep0_buf, descr_ptr, len);
					descr_ptr += len;
					break;
				}
				case USB_SET_ADDRESS:
					dev_addr = (uint8_t)(setup_value & 0xFF);
					break;
				case USB_GET_CONFIGURATION:
					ep0_buf[0] = dev_config;
					if (setup_len > 1)
						setup_len = 1;
					break;
				case USB_SET_CONFIGURATION:
					dev_config = (uint8_t)(setup_value & 0xFF);
					cdc_state = 3; /* configured */
					ep3_kick();    /* flush pre-enumeration backlog */
					break;
				case USB_GET_INTERFACE:
					ep0_buf[0] = 0;
					if (setup_len > 1)
						setup_len = 1;
					break;
				case USB_SET_INTERFACE:
					break;
				case USB_CLEAR_FEATURE:
					if ((setup_type & 0x1Fu) == 0x02u &&
					    (setup_value & 0xFF) == 0x00u) {
						uint8_t ep = setup_index & 0xFF;
						if (ep == (0x80u | EP1))
							UD_UEPn_TX_CTRL(EP1) = UEP_T_RES_NAK;
						else if (ep == EP2)
							UD_UEPn_RX_CTRL(EP2) = UEP_R_RES_ACK;
						else if (ep == (0x80u | EP3))
							UD_UEPn_TX_CTRL(EP3) = UEP_T_RES_NAK;
						else
							errflag = 0xff;
					} else {
						errflag = 0xff;
					}
					break;
				case USB_SET_FEATURE:
					if ((setup_type & 0x1Fu) == 0x02u &&
					    (setup_value & 0xFF) == 0x00u) {
						uint8_t ep = setup_index & 0xFF;
						if (ep == (0x80u | EP1))
							UD_UEPn_TX_CTRL(EP1) = UEP_T_RES_STALL;
						else if (ep == EP2)
							UD_UEPn_RX_CTRL(EP2) = UEP_R_RES_STALL;
						else if (ep == (0x80u | EP3))
							UD_UEPn_TX_CTRL(EP3) = UEP_T_RES_STALL;
						else
							errflag = 0xff;
					} else {
						errflag = 0xff;
					}
					break;
				case USB_GET_STATUS:
					ep0_buf[0] = 0;
					ep0_buf[1] = 0;
					if (setup_len > 2)
						setup_len = 2;
					break;
				default:
					errflag = 0xff;
					break;
				}
			}

			if (errflag == 0xff) {
				UD_UEPn_TX_CTRL(0) = UEP_T_TOG | UEP_T_RES_STALL;
				UD_UEPn_RX_CTRL(0) = UEP_R_TOG | UEP_R_RES_STALL;
			} else {
				if (setup_type & USB_REQ_TYP_IN) {
					len = setup_len > EP0_SIZE ? EP0_SIZE : setup_len;
					setup_len -= len;
					UD_UEPn_TX_LEN(0) = len;
					UD_UEPn_TX_CTRL(0) = UEP_T_TOG | UEP_T_RES_ACK;
				} else {
					if (setup_len == 0) {
						UD_UEPn_TX_LEN(0) = 0;
						UD_UEPn_TX_CTRL(0) = UEP_T_TOG | UEP_T_RES_ACK;
					} else {
						UD_UEPn_RX_CTRL(0) = UEP_R_TOG | UEP_R_RES_ACK;
					}
				}
			}
			break;
		}

		case UIS_TOKEN_SOF:
		default:
			break;
		}
		UD_INT_FG = UIF_TRANSFER;
	} else if (intflag & UIF_BUS_RST) {
		dev_config = 0;
		dev_addr = 0;
		setup_len = 0;
		UD_DEV_ADDR = 0;
		ep_init();
		cdc_state = 2; /* reset received: enumeration in progress */
		UD_INT_FG = UIF_BUS_RST;
	} else {
		UD_INT_FG = intflag;
	}
}

/* ---- console TX path ---- */
void usbfs_cdc_putc(char c)
{
	if (!clk_ok)
		return;

	/* ring push; drop new bytes on overflow (host absent), counted */
	uint16_t nh = tx_head + 1u;
	if (nh == CDC_RING_SIZE)
		nh = 0;
	if (nh == tx_tail) {
		cdc_drops++;
		return;
	}
	tx_ring[tx_head] = (uint8_t)c;
	tx_head = nh;

	ep3_kick();
}

/* ---- init ---- */
void usbfs_cdc_init(void)
{
	uint32_t sws = (RCC_CFGR0 >> 2) & 3u;

	cdc_state = 1;
	clk_ok = false;

	/* USBFS needs an exact 48 MHz clock: PLL/3 in the 144 MHz PLL regime.
	 * The factory program boots with SYSCLK on the 144 MHz PLL (SWS=2).
	 * Any other regime: skip USB - reconfiguring the PLL here would ripple
	 * through the S3K clock budget and the verified STK backend. */
	if (sws != 2u)
		return;

	RCC_CFGR0 = (RCC_CFGR0 & ~RCC_CFGR0_USBFSCLK_MASK) |
		    RCC_CFGR0_USBFSCLK_PLLDIV3;
	RCC_AHBPCENR |= RCC_AHBPCENR_USBFS;
	(void)RCC_AHBPCENR; /* read-back so the bus clock takes effect */

	clk_ok = true;

	UD_BASE_CTRL = UC_RESET_SIE | UC_CLR_ALL;
	delay_stk(180); /* >= 10 us @ 18 MHz */
	UD_BASE_CTRL = 0x00;
	UD_INT_EN = UIF_BUS_RST | UIE_TRANSFER; /* no SUSPEND: no sleep support */
	UD_BASE_CTRL = UC_DEV_PU_EN | UC_INT_BUSY | UC_DMA_EN;
	ep_init();
	UD_UDEV_CTRL = UD_PD_DIS | UD_PORT_EN;

	/* PFIC: enable IRQ 83. MIE stays 0 until the first mret into user
	 * mode (trap.S latches MPIE; mret promotes it), so no USBFS interrupt
	 * can hit the boot path - enumeration below polls INT_FG directly. */
	PFIC_IENR(USBFS_IRQn >> 5) |= (1u << (USBFS_IRQn & 31u));
}

void usbfs_cdc_enum_wait(void)
{
	uint32_t start;
	if (!clk_ok)
		return;

	start = STK_CNTL;
	/* ~2 s @ 18 MHz: generous for cable-attached enumeration (~100 ms)
	 * and the fixed boot overhead when no cable is present. */
	while (cdc_state < 3u) {
		usbfs_cdc_irq();
		if ((uint32_t)(STK_CNTL - start) >= 36000000u)
			break;
	}
	cdc_enum_ticks = (uint32_t)(STK_CNTL - start);
}