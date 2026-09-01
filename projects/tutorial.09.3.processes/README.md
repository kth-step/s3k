# Tutorial 09: Three Processes

## What this tutorial is about

This tutorial scales the previous ideas to three processes. App1 acts as the controller and dynamically redistributes timing budget between app2 and app3 so you can observe changing execution shares at runtime.

## Lecture Notes

By this point the important question is no longer "can S3K run more than one process?" but "how does S3K let one trusted component manage a partitioned system in a predictable way?"

This example answers that question through dynamic timing redistribution:

- app1 starts and configures app2 and app3,
- app1 controls the timing-slice capabilities associated with them,
- the budget assigned to app2 and app3 changes over time,
- the serial output makes the changing allocation visible.

The tutorial is useful because it shows that S3K's temporal model is not just static partitioning. Time can be redistributed, but only through explicit capability operations. There is no hidden scheduler policy silently changing priorities behind the application's back.

## What to look at in the code

Read:

- [app1/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.09.3.processes/app1/main.c)
- [app2/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.09.3.processes/app2/main.c)
- [app3/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.09.3.processes/app3/main.c)

Look for the timing-slice revoke, derive, and grant steps in app1, and compare them with the simpler continuously running loops in app2 and app3.

## Why this matters in S3K

This tutorial brings together several strands of the course:

- explicit authority,
- separation between control and work,
- analyzable temporal behavior,
- multi-core or multi-process partitioning as a first-class concern.

## How to run it

```bash
./scripts/docker.sh
cd projects/tutorial.09.3.processes
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```

## Expected result

You should see app2 and app3 continue running while app1 periodically changes how much timing budget each one receives.
