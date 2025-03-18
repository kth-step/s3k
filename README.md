S3K - Simple Secure Separation Kernel
=====================================

S3K is a capability-based separation kernel targetting embedded RISC-V systems.

Check [S3K examples](https://github.com/kth-step/s3k-examples) and [S3K PoC](https://github.com/kth-step/s3k-poc) to see how to integrate the kernel in a project.

The [S3K user library](https://github.com/kth-step/libs3k) defines basic C handlers for S3K system calls.
The [S3K User Manual](https://github.com/kth-step/libs3k/blob/main/docs/manual.pdf) describes the kernel's design and usage.

Repository structure
--------------------

- src - Source files.
- include - Header files.
- cross - Files for setting up cross compilation with meson.
- platform - Platform specific source files.
- meson.build - Meson 
- meson.options - Used to configure kernel parameters.
- Doxyfile - Configuration file for doxygen used to generate [documentation](https://kth-step.github.io/s3k).
- LICENSE - MIT License file

