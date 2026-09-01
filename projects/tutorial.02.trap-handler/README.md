# Tutorial 02: Trap Handler

## What this tutorial is about

This tutorial shows how an S3K application installs its own trap handler and inspects the resulting machine state when an exception occurs.

In a separation kernel, fault containment matters as much as normal execution. A process must be able to fail in a controlled way, and the fault information must remain local and analyzable.

## Lecture Notes

S3K keeps the process model intentionally small. Each process has a fixed execution context and a small set of virtual registers that can be configured explicitly. Among them are the registers used to define the trap program counter and trap stack.

This example configures a private trap path with:

- `s3k_vreg_set(S3K_VREG_TPC, ...)`
- `s3k_vreg_set(S3K_VREG_TSP, ...)`

When the exception is triggered, the handler prints values such as:

- `epc`: where the fault happened,
- `esp`: the saved stack pointer,
- `ecause`: why the trap occurred,
- `eval`: an exception-specific value.

The main lesson is that exception handling is part of the isolated process contract. S3K does not hide this machinery behind a large runtime. The application is responsible for making the trap path explicit.

## Short Note: Separate Trap Stack

The tutorial allocates a dedicated `trap_stack` buffer and registers it through `S3K_VREG_TSP` instead of reusing the normal application stack. This is good practice because the regular stack may already be corrupted, exhausted, or in an unexpected state when the exception happens.

Using a separate buffer makes the trap path more robust and easier to reason about: the handler starts from a known stack region that was reserved specifically for exception handling.

## Short Note: `interrupt("machine")`

The handler is declared with `__attribute__((interrupt("machine")))`. This is a compiler annotation that tells the RISC-V toolchain to treat the function differently from an ordinary C function.

Without this annotation, the function body might compile, but the generated entry and return code would not match the requirements of a real trap handler.

## What to look at in the code

Read [app1/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.02.trap-handler/app1/main.c). Focus on:

- trap handler registration through virtual registers,
- the deliberate exception path,
- the printed trap-state fields.

## How to run it

```bash
./scripts/docker.sh
cd projects/tutorial.02.trap-handler
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```

## Expected result

The program should trigger the handler and print the captured trap information. This is your first example of S3K exposing low-level control directly to the application instead of burying it inside a general OS abstraction.
