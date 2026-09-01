# Tutorial 07: IPC Without Yield

## What this tutorial is about

This tutorial revisits IPC with a different communication mode. Instead of the yielding synchronous pattern from tutorial 06, it uses asynchronous IPC without the yield flag and shows how that changes coordination between the two processes.

## Lecture Notes

The point of this tutorial is comparison. S3K IPC is not only about moving data; it also encodes scheduling intent. When the yield behavior changes, the structure of the whole interaction changes.

Here app1 still creates the communication endpoints and still starts app2, but the exchange pattern is different:

- the IPC mode is asynchronous,
- the endpoint is not created with yield semantics,
- both sides use `replyrecv()` in a different control flow than tutorial 06,
- app1 explicitly manages more of the scheduling and timing behavior.

This makes the tutorial a good study in design tradeoffs:

- synchronous yielding IPC is simpler when you want a tightly coupled request/reply handoff,
- asynchronous or non-yielding IPC gives more flexibility, but it also makes coordination more explicit in user code.

## What to look at in the code

Read:

- [app1/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.07.ipc-noyield/app1/main.c)
- [app2/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.07.ipc-noyield/app2/main.c)

Look for:

- `S3K_IPC_MODE_ASYNC`
- the absence of yield-oriented IPC flags,
- the explicit timing-slice handling and polling for blocked state.

## Why this matters in S3K

S3K makes policy visible. This tutorial shows that the kernel does not force one single communication style. Instead, it offers structured building blocks whose consequences are easy to reason about once you understand the capability and scheduling model.

## How to run it

```bash
./scripts/docker.sh
cd projects/tutorial.07.ipc-noyield
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```

## Expected result

You should see the two processes exchange messages successfully, but with a control flow that differs noticeably from tutorial 06. Reading the code side by side is strongly recommended.
