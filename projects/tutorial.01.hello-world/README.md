# Tutorial 01: Hello World

## What this tutorial is about

This is the smallest useful S3K application. Its job is simple: obtain access to the UART through a boot capability, enable that access in the local PMP configuration, synchronize the hardware state, and print a message.

The small size is the point. S3K is a separation kernel, not a general-purpose operating system. It does not start by giving an application a large runtime environment, a filesystem, a network stack, or a POSIX process model. Instead, it gives a process a minimal execution context plus a small set of explicit capabilities.

## Lecture Notes

S3K is designed around separation: each process gets a tightly controlled view of memory, devices, and execution time. A process can only use a resource if it holds a capability for it, and if the corresponding hardware state has been programmed consistently.

In this first example, the boot code has already created a capability for the UART MMIO region. The application:

- takes the boot UART memory capability,
- installs it into a PMP slot with `s3k_mem_pmp_set()`,
- calls `s3k_sync()` so the hardware view matches the kernel view,
- writes to the serial port.

This shows an important S3K idea very early: even "printing hello" is a resource-management operation. Device access is not ambient authority. It is explicit, auditable, and tied to a capability.

## Short Note: What Is PMP?

PMP stands for Physical Memory Protection. It is a RISC-V hardware mechanism that lets privileged software define which physical address ranges a process may read, write, or execute.

In S3K, capabilities describe which resources a process should be allowed to use, while PMP is one of the hardware mechanisms used to enforce those decisions. In this tutorial, the UART MMIO region is not usable until the corresponding capability is installed into a PMP entry and synchronized.

## What to look at in the code

Read [app1/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.01.hello-world/app1/main.c). The most important concepts are:

- `S3K_BOOT_MEM_UART_IDX`: the boot capability index for the UART device region.
- `s3k_mem_pmp_set()`: grants this process access to that MMIO range through a PMP entry.
- `s3k_sync()`: applies deferred resource state updates.

## How to run it

Use the Docker environment so the expected cross toolchain and `picolibc` setup are available:

```bash
./scripts/docker.sh
cd projects/tutorial.01.hello-world
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```

## Expected result

You should see the tutorial print on the serial console. If this works, you have validated the full minimal path of:

- boot capability creation,
- PMP programming,
- hardware synchronization,
- device use from an isolated process.
