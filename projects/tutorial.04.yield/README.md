# Tutorial 04: Yield

## What this tutorial is about

This tutorial boots a second process from the first one and then uses monitor-mediated yielding so the two processes can take turns executing.

It is the first example where S3K is used as an actual multi-process separation kernel rather than a single isolated application environment.

## Lecture Notes

S3K uses a static process model: the set of processes is known at build time, and each process has a well-defined memory region and execution context. One process can hold a monitor capability for another process. That monitor capability does not turn the holder into a general scheduler; it grants a specific and controlled form of management authority.

In this tutorial, app1 uses its monitor capability for app2 to:

- derive memory for app2,
- give app2 access to the UART device,
- set app2's initial program counter and state,
- resume app2,
- yield execution to it.

This illustrates a central design idea in S3K: control is explicit and capability-scoped. Process creation and activation are not hidden behind a heavyweight runtime service. They are assembled from precise kernel operations.

## What to look at in the code

Read:

- [app1/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.04.yield/app1/main.c)
- [app2/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.04.yield/app2/main.c)

Important operations include:

- `s3k_mon_mem_derive()`
- `s3k_mon_mem_pmp_set()`
- `s3k_mon_reg_set()`
- `s3k_mon_resume()`
- `s3k_mon_yield()`

## Why this matters in S3K

This is where the distinction between a microkernel and a separation kernel becomes concrete. S3K is not trying to implement a feature-rich process subsystem. It provides the minimal primitives needed to construct strongly isolated process interactions with clear authority boundaries.

## How to run it

```bash
./scripts/docker.sh
cd projects/tutorial.04.yield
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```

## Expected result

You should see output from both applications, with execution explicitly handed from one to the other. That handoff is not accidental scheduling; it is deliberate monitor-controlled transfer.
