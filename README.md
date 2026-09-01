# S3K - Safe and Secure Separation Kernel

[![CI Build](https://github.com/HAKarlsson/s3k/actions/workflows/build.yml/badge.svg)](https://github.com/HAKarlsson/s3k/actions/workflows/build.yml)

S3K is an experimental bare-metal capability-based separation kernel for RISC-V, built for safety- and security-critical systems such as avionics.
It supports dynamic partitioning with strict spatial and temporal isolation. The kernel is implemented in C and assembly with a small trusted computing base (TCB) and minimal hardware requirements (RISC-V PMP, machine mode and user mode).

Trusted user processes can dynamically create and destroy partitions. Time is a first-class capability for time-driven cyclic schedules. Memory capabilities configure the RISC-V PMP for dynamic spatial isolation. Monitor capabilities let trusted processes supervise and reconfigure others (boot, health checks, recovery). IPC capabilities move data, capabilities, and execution time donations between partitions.

Using the temporal fence instruction (see [PULP's CVA6 implementation](https://github.com/pulp-platform/cva6)), S3K provides time protection between partitions, reducing covert channels from core-local microarchitectural state. To mitigate off-core channels, pair S3K with hardware or platform measures such as cache partitioning, scratchpad/SRAM usage, or flushing shared resources on partition switches.

## Repository Organization

- `cross/` : Cross-compilation configuration files.
- `kern/` : Kernel source code and header files.
- `lib/` : Kernel API library (libs3k).
- `platform/` : Platform-specific kernel configurations and linker scripts.
- `projects/` : Example projects demonstrating S3K capabilities.
- `scripts/` : Build and development scripts (e.g., Docker wrapper).
- `API.md` : Kernel API documentation.
- `LICENSE` : License file.
- `meson.build` : Meson build configuration.
- `meson.options` : Kernel build options.
- `README.md` : This file.

## Compilation instructions

Requirements: Docker (rootless).
The repo-managed container image installs the extra tooling needed by the filesystem tutorial targets, including `mtools`, and defaults to `linux/amd64` for compatibility with the published toolchain image.
The Docker wrapper also publishes container port `5555` to the host by default so the networking tutorial can be reached from macOS or Linux.

### Compiling only the kernel

```bash
./scripts/docker.sh
meson setup builddir --cross-file=cross/rv64imac.ini
ninja -C builddir
```

## Compilation instructions for hello project

```bash
./scripts/docker.sh
cd projects/tutorial.01.hello-world
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```

## Tutorial index

- [tutorial.01.hello-world](projects/tutorial.01.hello-world/README.md): minimal UART-enabled application that prints a greeting.
- [tutorial.02.trap-handler](projects/tutorial.02.trap-handler/README.md): install a trap handler and inspect exception state.
- [tutorial.03.mem-cap](projects/tutorial.03.mem-cap/README.md): derive and configure memory capabilities with PMP.
- [tutorial.04.yield](projects/tutorial.04.yield/README.md): start a second process and yield execution to it.
- [tutorial.05.timing](projects/tutorial.05.timing/README.md): derive or grant timing budget between processes.
- [tutorial.06.ipc](projects/tutorial.06.ipc/README.md): synchronous IPC between a client and server process.
- [tutorial.07.ipc-noyield](projects/tutorial.07.ipc-noyield/README.md): asynchronous IPC without the yield flag.
- [tutorial.08.ipc-cap](projects/tutorial.08.ipc-cap/README.md): transfer a memory capability over IPC.
- [tutorial.09.3.processes](projects/tutorial.09.3.processes/README.md): run three processes and rebalance timing budget dynamically.
- [tutorial.10.fs](projects/tutorial.10.fs/README.md): create a disk image, run the filesystem demo, then inspect the written file with `ninja -C builddir disk-read`.
- [tutorial.11.network](projects/tutorial.11.network/README.md): run the telnet-based integer service and connect from the host on port `5555`.

## Future Work

S3K is a research prototype under active development. We have many ideas for improvements but cannot guarantee timelines.

- **Formal verification** [Ongoing] 
  - Formalizing core kernel abstractions and proving security/safety properties using HOL4.
  - Extending verification to cover the kernel implementation (binary level).
- **Kernel reimplementation** [Ongoing]
  - Reimplementing the kernel in a formally-verifiable language designed for safety-critical systems.
- **Extended platform support**
  - Add 32-bit RISC-V support and additional boards/SoCs.
- **Fine-grained monitor capabilities**
  - Restrict monitor privileges for more granular partition management.
- **Asynchronous IPC**
  - Support message queues alongside current synchronous IPC and message buffers.
- **Interrupt capabilities**
  - Let partitions handle hardware interrupts with maintained isolation and security.
- **Performance testing and benchmarking**
  - Establish benchmark suites and comprehensive test coverage.
- **Documentation and examples**
  - Expand API documentation and add example projects.
- **OS partition support**
  - Run real-time operating systems and Linux as isolated partitions within S3K.
- **User-space library**
  - Provide a higher-level abstraction over the kernel API for application developers.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgements

This work was supported by KTH CDIS (Centre for Cyber Defence and Information Security) and the Intel Corporation.

## Related Papers

H. Karlsson, and R. Guanciale, "Partitioning Kernel with Capability Controlled Temporal and Spatial Partitioning," in 2025 IEEE 46th Real-Time Systems Symposium (RTSS'25), Dec. 2025.

H. A. Karlsson, “Minimal partitioning kernel with time protection
and predictability,” in Proc. IEEE European Symposium on Security
and Privacy Workshops (EuroS&PW’24). IEEE, 2024, pp. 234–241.

K. Palmskog, et al. "Trustworthy Verification of RISC-V Binaries Using Symbolic Execution in HolBA." arXiv preprint arXiv:2503.14135 (2025).
