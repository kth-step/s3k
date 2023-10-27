.globl monitor_bin, monitor_bin_len
.globl crypto_bin, crypto_bin_len
.globl uart_bin, uart_bin_len

.section .data

monitor_bin:
.incbin "build/monitor.bin"
crypto_bin:
.incbin "build/crypto.bin"
uart_bin:
.incbin "build/uartppp.bin"
end_bin:

monitor_bin_len:
.dword (crypto_bin - monitor_bin)
crypto_bin_len:
.dword (uart_bin - crypto_bin)
uart_bin_len:
.dword (end_bin - uart_bin)
