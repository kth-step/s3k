# S3K

[![CI Build](https://github.com/HAKarlsson/s3k/actions/workflows/build.yml/badge.svg)](https://github.com/HAKarlsson/s3k/actions/workflows/build.yml)

S3K is an experimental bare-metal capability-based partitioning kernel for RISC-V, targeting safety- and security-critical applications, particularly in the avionics domain.
The kernel is designed to follow the ARINC 653 standard for avionics in terms of spatial and temporal isolation of partitions.
Furthermore, S3K incorporates mechanisms for time protection partitions, effectively preventing timing side-channel attacks.

The capabilities of S3K allow trusted user processes to dynamically create and destroy partitions.
Time is treated as a first-class capability, allowing processes to precisely specify when they are executed in a time-driven cyclic schedule.
Memory capabilities allow processes to configure the RISC-V Physical Memory Protection (PMP) unit, enabling dynamic spatial isolation.
Monitor capabilities allow processes to monitor and configure other processes, enabling the creation of system monitors that oversee the state of the system, boot the system, and reconfigure the system dynamically.
Lastly, IPC capabilities allow processes to communicate with each other, sending data, capabilities, and even execution time.


## Repository Organization

- `cross/` : Files related to cross compilation.
- `include/` : Kernel header files.
- `platform/` : Platform specific kernel configuration files.
- `src/` : Kernel source files.
- `lib/` : Kernel API library.
- `projects/` : Example projects.
- `Doxygen` : Doxygen configuration file.
- `LICENSE` : License file.
- `meson.build` : Meson build configuration.
- `README.md` : This README file.
- `API.md` : Kernel API description.
- `meson.options` : Kernel configuration file.

## Compilation instructions

Requirements: Docker (rootless).

### Compiling only the kernel

```bash
./docker.sh
meson setup builddir --cross-file=cross/rv64imac.toml
ninja -C builddir
```

## Compilation instructions for hello project

```bash
./docker.sh
cd projects/hello
meson setup builddir --cross-file=../../cross/rv64imac.toml
ninja -C builddir
ninja -C builddir qemu-run
```
