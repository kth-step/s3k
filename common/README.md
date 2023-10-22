# S3K Userland library

## Drivers
Various drivers.
- `inc/drivers/uart.h`: UART driver header file.
- `src/drivers/uart`: Implementation of UART drivers
    - `ns16550a`: Driver for NS16550A UART. Used in qemu-system-riscv64 virt.
    - `sifive`: Driver for SiFive UART. Used in qemu-system-riscv64 sifive\_u
      and in HiFive Unleashed FU540.

## libaltc
Small userland C library for embedded applications.
- `altio.h`: Puts, gets and limited printf.
- `string.h`: memchr, memcmp, memcpy, memmove and memset.
