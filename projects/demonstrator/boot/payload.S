

.globl monitor_bin, monitor_bin_len
.globl crypto_bin, crypto_bin_len
.globl uart_bin, uart_bin_len

.section .data

#define STR(x) #x
#if defined(PLATFORM_qemu_virt)
#define PATH(x) STR(build/qemu_virt/x.bin)
#elif defined(PLATFORM_sifive_unleashed)
#define PATH(x) STR(build/sifive_unleashed/x.bin)
#endif

monitor_bin:
.incbin PATH(monitor)
crypto_bin:
.incbin PATH(crypto)
uart_bin:
.incbin PATH(uartppp)
end_bin:

monitor_bin_len:
.dword (crypto_bin - monitor_bin)
crypto_bin_len:
.dword (uart_bin - crypto_bin)
uart_bin_len:
.dword (end_bin - uart_bin)
