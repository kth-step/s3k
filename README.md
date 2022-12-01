# S3K - Simply Secure Separation Kernel

Simply Secure Separation Kernel (S3K) is a real-time separation kernel for embedded RISC-V applications. S3K aims to spatially and temporally isolate all partitions running on the RISC-V processor. S3K relies on RISC-V's PMP mechanism to isolate the partitions, no virtual memory is required.


- **Formal Verification.** A separation kernel should be completely verified. We consider the following two approaches:

    1. We formally verify the C code with respect to a formal model of the kernel and the RISC-V specification. This allows us to compile the kernel with formally verified compiler [CompCert](https://github.com/AbsInt/CompCert), and get a fully formally verified kernel.

    2. We formally verify the kernel binary using [HolBA](https://github.com/kth-step/HolBA), allowing us to use any compiler and thus get better optimization.

- **Security.** A separation kernel should secure processes with spatial and temporal isolation.

- **Performance.** Fast context switches and IPC calls are essential for a separation kernel.

- **Dynamic.** The kernel should have dynamic memory protection and scheduling, allowing the system to be updated during runtime.

- **Small footprint.** A small footprint is important for most embedded systems.

- **Extensible.** RISC-V has a lot of extensions (official and custom) and these extension can invalidate the kernel verification. We want to synthesize kernel code that extends the kernel functionality and verification.

- **Real-time properties.** Real-time properties makes the kernel suitable for some RT applications. Exact properties TBD.

## Documentation

- [S3K - API](doc/API.md)
- [S3K - INSTALL](doc/INSTALL.md)
- [S3K - PLAN](doc/PLAN.md)
- [S3K - Proof-of-Concept](https://github.com/kth-step/s3k-poc)

## Related Projects 
- [OpenMZ](https://github.com/castor-software/openmz) - A lightweight open-source implementation of MultiZone Security.
- [seL4](https://sel4.systems/) - A verified and secure µ-kernel, suitable for more capable hardware.
- [eChronos](https://github.com/echronos/echronos) - a real-time operating systems.
- [CertiKOS](http://flint.cs.yale.edu/certikos/) - A verified, secure and modularized kernel for more capable hardware.
- [Keystone Enclave](https://github.com/keystone-enclave/keystone) - A lightweight TEE for RISC-V.
- [MultiZone Security](https://hex-five.com) - A lightweight TEE for RISC-V.
- [STH](https://bitbucket.org/sicssec/sth/src/master/) - SICS Thin Hypervisor, a research kernel part of the PROSPER project.

