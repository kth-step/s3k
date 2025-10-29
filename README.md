S3K - Simple Secure Separation Kernel
=====================================

S3K is a capability-based separation kernel targetting embedded RISC-V systems.

Documentation
-------------

| Page | Description |
| --- | --- |
| [Home](https://github.com/kth-step/s3k/wiki)                  | Documentation Index |
| [S3K Design](https://github.com/kth-step/s3k/wiki/S3K-Design) | High-level design of S3K |
| [S3K Implementation](https://github.com/kth-step/s3k/wiki/S3K-Implementation) | Description of S3K implementation |
| [S3K API](https://github.com/kth-step/s3k/wiki/S3K-API)       | User-level Kernel API |

More documenation will be added.

Configuration
-------------

Set your compiler toolchain in `tools.mk`. By default we have the following:
```
CC      =riscv64-unknown-elf-gcc
AR      =riscv64-unknown-elf-ar
LD      =riscv64-unknown-elf-ld
SIZE    =riscv64-unknown-elf-size
OBJDUMP =riscv64-unknown-elf-objdump
OBJCOPY =riscv64-unknown-elf-objcopy
```

Build and Run
-------------

For building and running the `hello` project
```bash
# Build common libraries kernel and hello project
cd projects/hello
# Build the project
make
# Run the program using QEMU
make qemu
```

Requirements
------------

- Docker container
The ``Dockerfile`` provides the reference to a container with RISC-V compiler
and qemu installed. If you are using a x64 machine this is the suggestedned
development environment. If you are using Mac, you can install both the compiler
and qemu-system via brew.

- [RISC-V GNU Toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain) 
  - We recommend that you clone the repository and build the toolchains as follows:
    ```
    ./configure --prefix=/opt/riscv --enable-multilib
    sudo make
    ```
    This puts `riscv64-unknown-elf-*` tools in `/opt/riscv/bin`.
  - You should then add `/opt/riscv/bin` to the `PATH` variable, e.g., `PATH=/opt/riscv/bin:$PATH`.
  - You can make the toolchain user only by setting `--prefix=$HOME/.opt/riscv` and `PATH=$HOME/.opt/riscv/bin:$PATH`.
    Then you do not need to build the toolchain with `sudo`.
- QEMU System RISC-V - Build or install QEMU with RISC-V.
  - You should have something like `qemu-system-riscv64` installed. It allows us to use our custom kernel.
  - You should not use `qemu-riscv64`, this only runs/simulates a Linux kernel.

Repository structure
--------------------

- kernel - The kernel source code.
  - src - C and assembly files
  - inc - Header files
  - linker.ld - Linker script
  - Makefile - Kernel's makefile
  - s3k_conf.h - Default kernel configuration
- common - Libraries for the kernel
  - inc - header files
    - drivers - UART and timer driver
    - s3k - Kernel API library
    - altc - Minimal library needed by the kernel
    - plat - Platform configurations
  - src - Source files for libraries in inc
    - start - startup files for applications
  - plat/<platform>.mk - Compiler flag and more for a platform.
- projects - Example projects using the kernel
  - demonstrator - bigger example project
  - hello - Hello, world example with two processes
  - ping-ping - IPC example
  - trapped - Trap handling example
- LICENSE - MIT License file
- tools.mk - Set the compiler tools here
