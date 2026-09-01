# Tutorial 05: Timing

## What this tutorial is about

This tutorial extends the previous multi-process setup with timing-slice capabilities. Instead of only deciding who may run, it also controls how much execution time is delegated.

## Lecture Notes

Temporal isolation is one of the defining goals of a separation kernel. Protecting memory is not enough if one process can consume all CPU time and starve another. S3K addresses this with explicit timing-slice capabilities and frame-based scheduling ideas inspired by safety-critical systems.

In this example, app1 manages app2's execution budget through monitor timing operations. The important idea is that time, like memory, is treated as an explicitly managed resource. A process cannot simply assume CPU time exists in an unlimited pool.

Relevant operations include:

- `s3k_mon_tsl_derive()`
- `s3k_mon_tsl_grant()`
- monitor resume and yield operations to coordinate execution.

The tutorial also highlights that timing control and process control are related but distinct:

- process-control authority says who can manipulate another process,
- timing authority says how much execution budget can be assigned.

## What to look at in the code

Read:

- [app1/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.05.timing/app1/main.c)
- [app2/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.05.timing/app2/main.c)

Focus on how app1 initializes app2 and then hands over timing resources before both processes execute on separate cores in QEMU.

## Why this matters in S3K

This is one of the clearest examples of the "safety plus security" flavor of S3K. The system is not only trying to stop unauthorized access; it is also trying to make execution behavior analyzable and partitionable.

## How to run it

```bash
./scripts/docker.sh
cd projects/tutorial.05.timing
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```

## Expected result

You should see both applications running, with the tutorial demonstrating that process startup and execution budget assignment are separate, explicit steps.
