# Tutorial 06: IPC

## What this tutorial is about

This tutorial introduces S3K inter-process communication. App1 sets up a server and client endpoint pair, grants the server endpoint to app2, and then performs request/reply exchanges in which app2 returns the square of the sent number.

## Lecture Notes

IPC in S3K is capability-based. A process cannot send to an arbitrary destination just because another process exists. It needs IPC capabilities that describe the communication object and the allowed direction of use.

This tutorial uses the synchronous, yielding style:

- app1 derives an IPC channel capability from a boot IPC capability,
- it derives a server socket and a client socket,
- it grants the server side to app2,
- app1 issues `s3k_ipc_call()`,
- app2 handles the request and waits for the next one with `s3k_ipc_replyrecv()`.

The "yielding" behavior matters. Communication is tied to scheduling behavior, so an IPC operation can also define who should run next. This is very different from treating IPC as just a byte stream between threads managed by a large background scheduler.

One special detail in this example is that app2 uses a dedicated calling convention for its entry point, receiving the server endpoint as an argument. That is intentional and part of the tutorial.

## What to look at in the code

Read:

- [app1/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.06.ipc/app1/main.c)
- [app2/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.06.ipc/app2/main.c)

Important concepts:

- `S3K_BOOT_IPC_0`
- `s3k_ipc_derive()`
- `s3k_ipc_call()`
- `s3k_ipc_replyrecv()`
- `s3k_mon_state_get()` used to observe when the server side is blocked and ready.

## Why this matters in S3K

This is the first tutorial where the kernel starts to look like a communication substrate rather than just an isolation substrate. Even so, the communication model remains explicit, bounded, and capability-controlled.

## How to run it

```bash
./scripts/docker.sh
cd projects/tutorial.06.ipc
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```

## Expected result

You should see request and response traffic between the two applications, with app2 computing and returning the square of the input values.
