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

## How to debug it with GDB

This tutorial is small enough that you can follow one application system call all the way into the kernel trap handler and back out again.

The most useful path to debug is:

- app breakpoint in `setup_uart()`, before the UART capability is installed
- kernel breakpoint in `syscall_mem_pmp_set()`, which handles the `s3k_mem_pmp_set()` system call

### 1. Build the tutorial

Start from the Ubuntu container so the expected `qemu-system-riscv64` and `riscv64-unknown-elf-gdb` tools are available:

```bash
./scripts/docker-ubuntu.sh
cd projects/tutorial.01.hello-world
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
```

### 2. Start QEMU with the GDB port open

Run QEMU in stopped mode with the GDB server listening on TCP port `3333`:

```bash
ninja -C builddir qemu-debug
```

This target comes from [projects/_mesonlib/tutorial_targets/meson.build](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/_mesonlib/tutorial_targets/meson.build). It starts QEMU with:

- `-gdb tcp::3333`
- `-S`

`-S` means the guest CPU does not start executing until GDB tells it to continue.

Keep this terminal open.

### 3. Attach GDB and load symbols

Open a second terminal in the same container and attach GDB:

```bash
cd projects/tutorial.01.hello-world
riscv64-unknown-elf-gdb
```

Inside GDB:

```gdb
set confirm off
target remote 127.0.0.1:3333
add-symbol-file builddir/subprojects/s3k/kern/s3k.elf
add-symbol-file builddir/app1/tutorial_app/app1.elf
```

That loads symbols for both:

- the kernel ELF: `builddir/subprojects/s3k/kern/s3k.elf`
- the application ELF: `builddir/app1/tutorial_app/app1.elf`

If you want the repo-provided helper target, this is equivalent to:

```bash
ninja -C builddir gdb-run
```

### 4. Add one app breakpoint and one kernel breakpoint

Set an application breakpoint in [app1/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.01.hello-world/app1/main.c:7), right before the app enters the syscall path:

```gdb
break setup_uart
```

Set a kernel breakpoint in [kern/src/syscall.c](/Users/guancio/Sources/STEP/sw/s3k.v2/kern/src/syscall.c:275):

```gdb
break syscall_mem_pmp_set
```

Now start execution:

```gdb
continue
```

GDB should stop first in `setup_uart()`.

### 5. Step through the app-side syscall boundary

From the app breakpoint, step until the `s3k_mem_pmp_set()` call in [app1/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.01.hello-world/app1/main.c:13):

```gdb
list
next
next
next
```

When you are on the `s3k_mem_pmp_set(...)` line, inspect the app stack and registers:

```gdb
bt
info registers sp ra a0 a1 a2 a3 a4
x/16gx $sp
```

At this point you are still on the application stack.

If you want to see the inline `ecall` itself, step into the wrapper in [lib/include/s3k/syscall.h](/Users/guancio/Sources/STEP/sw/s3k.v2/lib/include/s3k/syscall.h:295):

```gdb
step
display/i $pc
next
next
next
next
stepi
```

The relevant instruction is the `ecall` at [lib/include/s3k/syscall.h](/Users/guancio/Sources/STEP/sw/s3k.v2/lib/include/s3k/syscall.h:302).

### 6. Stop in the kernel and inspect the kernel stack

After the `ecall`, execution enters the trap path in [kern/src/trap.S](/Users/guancio/Sources/STEP/sw/s3k.v2/kern/src/trap.S:73) and then lands in `syscall_mem_pmp_set()`.

If the breakpoint in `syscall_mem_pmp_set()` does not trigger immediately because you stepped instruction-by-instruction, use:

```gdb
continue
```

Once stopped in the kernel:

```gdb
bt
frame 0
info registers sp ra a0 a1 a2 a3 a4 tp
x/16gx $sp
```

Now you are looking at the kernel stack, not the application stack.

The trap dispatch that routes an `ecall` into the syscall handler is in [kern/src/trap.S](/Users/guancio/Sources/STEP/sw/s3k.v2/kern/src/trap.S:78). The syscall implementation you are stopped in is [kern/src/syscall.c](/Users/guancio/Sources/STEP/sw/s3k.v2/kern/src/syscall.c:275), which calls `mem_pmp_set(...)` and returns to the current process.

### 7. Compare the app stack and the kernel stack

The clearest way to see the stack switch is:

1. stop in `setup_uart()`
2. note `$sp` and dump memory with `x/16gx $sp`
3. continue into `syscall_mem_pmp_set()`
4. note `$sp` again and dump `x/16gx $sp`

You should see a different stack pointer value and a different stack layout after the trap, because the kernel is executing on its own saved context.

### 8. Finish stepping back to the app

From the kernel breakpoint:

```gdb
finish
continue
```

You can then break again in `main()` or let the tutorial run to completion:

```gdb
break main
continue
```

If everything is working, QEMU will eventually print:

```text
uart set up!
hello, world
```

### 9. Useful GDB commands summary

```gdb
target remote 127.0.0.1:3333
add-symbol-file builddir/subprojects/s3k/kern/s3k.elf
add-symbol-file builddir/app1/tutorial_app/app1.elf
break setup_uart
break syscall_mem_pmp_set
continue
bt
info registers sp ra a0 a1 a2 a3 a4 tp
x/16gx $sp
step
next
stepi
finish
```

## Expected result

You should see the tutorial print on the serial console. If this works, you have validated the full minimal path of:

- boot capability creation,
- PMP programming,
- hardware synchronization,
- device use from an isolated process.
