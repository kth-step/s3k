s3k - Simple Secure Separation Kernel
=====================================

s3k is a separation kernel targetting embedded RISC-V systems.

- [Documentation](https://kth-step.github.io/s3k/) built with doxygen.


Repository structure
--------------------
- kernel - The kernel source code.
  - src - C and assembly files
  - inc - Header files
  - linker.ld - Linker script
  - Makefile - Kernel's makefile
  - s3k_conf.h - Default kernel configuration
- plat64 - Configuration for RISC-V 64-bit platforms.
  - inc/plat - Platform configurations as header files
  - config.mk - For compiler flags.
- projects - Example projects using the kernel
  - demonstrator - bigger example project
  - hello - Hello, world example with two processes
  - ping-ping - IPC example
  - trapped - Trap handling example
  - wcet - deprecated project (for now)
- userland - Userland libraries for the project applications
  - inc - header files
    - drivers - UART driver
    - s3k - Kernel API
    - s3klib - Userland library
  - src - C and assembly files
    - s3k - Kernel API
    - s3klib - Userland library
    - start - application boot code
- API.md - Kernel API
- LICENSE - MIT License file
- tools.mk - Set the compiler tools here

Configuration
-------------

s3k is configured by using `config.mk`.
