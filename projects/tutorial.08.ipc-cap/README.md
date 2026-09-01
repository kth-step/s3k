# Tutorial 08: IPC With Capability Transfer

## What this tutorial is about

This tutorial extends IPC one step further: the client does not only send data, it transfers authority. App1 derives a memory capability, sends it to app2 through IPC, and app2 maps that memory and reads from it before replying.

## Lecture Notes

This is one of the most important tutorials in the set because it shows what capability-based communication really buys you. In many systems, data sharing and authority sharing are separate mechanisms, often glued together informally. In S3K they are unified.

The message format can carry both ordinary payload data and a capability reference. In this example:

- app1 derives a memory capability from its RAM authority,
- IPC is configured with memory-transfer support,
- app1 sends the capability to app2,
- app2 installs the received memory capability into PMP,
- app2 reads the shared content and replies,
- app1 revokes the transferred descendants when the test ends.

Revocation is a major concept here. Capability transfer is safe only if the system can reason about who derived what from whom. The tutorial uses `s3k_mem_revoke()` to reclaim authority below the transferred memory capability.

## What to look at in the code

Read:

- [app1/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.08.ipc-cap/app1/main.c)
- [app2/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.08.ipc-cap/app2/main.c)

Pay attention to:

- `S3K_IPC_FLAG_MEM`
- the message capability fields,
- PMP setup on the receiver side,
- the revoke step on the sender side.

## Why this matters in S3K

This tutorial captures the deeper design of S3K: isolation is not only about denying access, but also about supporting controlled delegation. A capability system becomes especially powerful when sharing, confinement, and revocation all fit the same model.

## How to run it

```bash
./scripts/docker.sh
cd projects/tutorial.08.ipc-cap
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```

## Expected result

You should see output showing that app2 received the transferred memory authority, mapped it successfully, and read the expected value before app1 revoked the delegated authority tree.
