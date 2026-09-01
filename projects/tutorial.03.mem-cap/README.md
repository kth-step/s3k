# Tutorial 03: Memory Capabilities

## What this tutorial is about

This tutorial introduces one of the core S3K mechanisms: memory capabilities. The application inspects a boot memory capability, derives a smaller capability from it, maps that derived region through PMP, and then writes into the authorized memory range.

## Lecture Notes

Capabilities in S3K are communicable and unforgeable tokens of authority. For memory, a capability describes a region and the permissions attached to it. A process can derive a smaller or weaker capability from a capability it already owns, but it cannot use derivation to gain more authority than it started with.

This is the key discipline behind spatial isolation:

- the kernel tracks derivation,
- authority only flows downward,
- revocation remains possible because the kernel knows the derivation structure.

The slides model the capability table as a nested stack. A capability does not only control a resource such as a RAM range; it also owns a slice of the kernel capability table that can be consumed to hold descendants. That is why the table metadata includes both resource information and derivation-fuel information.

This tutorial demonstrates the basic memory operations:

- `s3k_mem_get()` to inspect a capability,
- `s3k_mem_derive()` to create a child capability,
- `s3k_mem_pmp_set()` to attach the capability to a PMP slot,
- `s3k_sync()` to make the PMP update effective.

In the lecture slides, the introspection operation is presented as `s3k_mem_read(i, cap)`: it copies the selected capability into a user-visible structure. In the current code and API this role is served by `s3k_mem_get()` and `s3k_mem_introspect()`, which let the application inspect the capability fields that the kernel is maintaining.

## Short Note: Effect of Each Operation

The slide examples are useful because they show not only what each operation means, but also how the capability table changes after the call.

- `s3k_mem_read(i, cap)` or, in the current API, `s3k_mem_get(i, &cap)`:
  effect: no authority changes. The kernel copies the capability metadata into a user-visible structure, but the table itself is unchanged.
- `s3k_mem_derive(0, 3, R, 7, 11)`:
  effect: a child capability is created with owner `1`, fuel `3`, free `3`, base `7`, size `11`, and permission `R`. The parent keeps its original resource extent but its remaining free count drops from `10` to `7`.
- deriving again from that child, as in the slides with `s3k_mem_derive(4, 1, R, 7, 10)`:
  effect: the child now has free `2` instead of `3`, and a grandchild appears with fuel `1`, free `1`, base `7`, size `10`, and permission `R`.
- `s3k_mem_delete(4)`:
  effect: the selected capability entry is deleted, but this is not a subtree operation. It removes that entry only; it does not recursively reclaim the whole descendant structure the way revoke does.
- `s3k_mem_revoke(i)`:
  effect: all descendants of capability `i` are removed and their fuel is reclaimed upward. In the slide sequence, the parent's free count increases step by step until it returns to the original `free = 10`, meaning the whole derived subtree has been reclaimed.

This is the behavior students should keep in mind while reading the tutorial code: derive consumes fuel and creates structure, get only observes, delete removes one entry, and revoke collapses an entire derived subtree back into the parent.

The code also uses `__payload` as a boundary marker. Conceptually, this is the end of the program image inside the process RAM window. The tutorial aligns upward from `__payload` to the next `0x10000` boundary, then derives a capability for that aligned region. The intent is to take a chunk of RAM that belongs to the process's boot RAM authority but is not occupied by the loaded program image, so it can safely be used as scratch memory for the demo.

## Short Note: Where the Initial Large RAM Capability Comes From

The root memory capability is not created by the tutorial itself. It is created by the kernel during boot.

The capability ordinal is exposed to applications in [lib/include/s3k/boot.h](/Users/guancio/Sources/STEP/sw/s3k.v2/lib/include/s3k/boot.h:11), where `S3K_BOOT_MEM_RAM_IDX` is defined as the first memory-capability block.

On `qemu_virt`, the actual RAM region behind that boot capability is initialized in [kern/platform/qemu_virt.c](/Users/guancio/Sources/STEP/sw/s3k.v2/kern/platform/qemu_virt.c:30). The platform provides:

- a large RAM memory capability rooted at `RAM_BASE = 0x80000000`,
- with `RAM_SIZE = 0x10000000`,
- inserted into the memory-capability table through `mem_init(init_mem)`.

The generic kernel logic that materializes those root capabilities is in [kern/src/mem.c](/Users/guancio/Sources/STEP/sw/s3k.v2/kern/src/mem.c:16). In particular, `mem_init()` places each initial capability at the start of a fuel-sized block in `mem_table`, with:

- `owner = 1`,
- `cfree = MAX_MEMORY_FUEL`,
- `csize = MAX_MEMORY_FUEL`.

So the tutorial begins with a large boot RAM capability that covers a whole platform RAM region. It then splits that capability and takes a smaller piece that starts after the already loaded application image.

This matches the sequence shown in the slides: start from one large root capability, derive a smaller one with reduced permissions or smaller extent, and continue deriving recursively if needed.

## Short Note: What Capability Fuel Means

The slides describe `cfree` as the remaining "fuel" of a capability. That is a good mental model: fuel is the budget that allows a capability to be split into more capabilities.

For memory capabilities, each entry stores:

- `cfree`: how much derivation fuel is still available,
- `csize`: the original amount of fuel assigned to that capability block.

You can see these fields in [lib/include/s3k/types.h](/Users/guancio/Sources/STEP/sw/s3k.v2/lib/include/s3k/types.h:155). When the tutorial calls `s3k_mem_derive(..., ram_fuel, ...)` with `ram_fuel = 2`, it is asking the kernel to spend two units of derivation fuel from the parent capability and assign those two units to the child capability.

Fuel is not the number of bytes in the memory region. The bytes are described by `base` and `size`. Fuel is metadata that controls derivation structure, bounded delegation, and revocation.

The examples in the slides show this visually:

- a root capability may begin with fuel 10 and free 10,
- deriving a child with fuel 3 reduces the parent's free count to 7,
- deriving again from that child reduces the child's free count further,
- revoking descendants returns that free count to the ancestor.

That is exactly the bookkeeping style used by the implementation here.

## Short Note: How the Capability Tree Is Implemented

In the lecture material, capabilities are presented as a derivation tree. In this kernel, that tree is implemented very compactly inside a flat table rather than with heap-allocated nodes and explicit child lists.

The core logic is in [kern/src/mem.c](/Users/guancio/Sources/STEP/sw/s3k.v2/kern/src/mem.c:106):

- `mem_table[i]` is the parent capability,
- deriving a child decreases `mem_table[i].cfree`,
- the child is then placed at index `j = i + mem_table[i].cfree`,
- the child receives its own `cfree` and `csize` block.

This means a capability owns a contiguous region of the capability table, and its descendants live inside that reserved range. Revocation works by walking back through that range, invalidating children, clearing any PMP state they installed, and adding their fuel back to the parent. That revoke path is implemented in `mem_revoke()` in the same file.

So the tree is real conceptually, but it is represented by contiguous slices of the table plus the `cfree/csize` bookkeeping, not by pointer-linked node objects.

This is also why the slide deck calls it a nested stack: the descendants of a capability occupy space inside the parent's reserved block, and reclaiming descendants is structurally similar to popping entries back out of that block.

## Short Note: Derive, Delete, and Revoke

Slides 7 through 15 distinguish three different ideas that are easy to blur together:

- `derive`: create a child capability inside the parent's authority and fuel budget,
- `delete`: remove one capability entry,
- `revoke`: remove all descendants of a capability and reclaim their fuel.

The important conceptual difference is that `delete` is local, while `revoke` is subtree-wide.

In the current kernel code, revocation is implemented directly in [kern/src/mem.c](/Users/guancio/Sources/STEP/sw/s3k.v2/kern/src/mem.c:141). The slide deck also notes an important behavior that the code preserves: revocation may be partial if preemption occurs. In that case the kernel can reclaim some descendants, stop, and return with work still left to do. You can see that in the loop around `preempt()` inside `mem_revoke()`.

## Short Note: NAPOT Encoding

RISC-V PMP stores address ranges in a hardware-specific format. For naturally aligned power-of-two regions, that format is called NAPOT, short for Naturally Aligned Power-Of-Two.

The tutorial uses:

- `s3k_pmp_napot_encode(base, size)` to convert a normal `(base, size)` pair into the PMP register format,
- `s3k_pmp_napot_decode_base()` and `s3k_pmp_napot_decode_size()` when printing the configured region.

This is why the tutorial aligns `ram_base` to `ram_size`. A NAPOT entry describes a power-of-two region aligned to its own size, so choosing `REGION_SIZE = 0x10000` and aligning the base to that same value makes the region valid for PMP programming.

## What to look at in the code

Read [app1/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.03.mem-cap/app1/main.c). Pay special attention to:

- `S3K_BOOT_MEM_RAM_IDX` and `S3K_BOOT_MEM_UART_IDX`,
- the printed memory-capability metadata,
- how the derived capability chooses a subrange,
- the write into memory after PMP authorization.

Two lines are especially important:

- [app1/main.c:100](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.03.mem-cap/app1/main.c:100) writes `0x10` to the first word of the derived RAM region at `ram_base`. This is the first direct proof that the newly derived capability plus the PMP update really gave the process access to that memory.
- [app1/main.c:101](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.03.mem-cap/app1/main.c:101) immediately reads the value back and prints it. Together, these two lines turn the whole capability-derivation sequence into an observable end-to-end test.

## Why this matters in S3K

General-purpose systems often make memory feel implicit. S3K does the opposite. Memory ownership, sub-allocation, and mapping are first-class kernel-managed operations. That explicit model is what later tutorials build on when they transfer memory between processes or delegate authority through monitor capabilities.

## How to run it

```bash
./scripts/docker.sh
cd projects/tutorial.03.mem-cap
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```

## Expected result

You should see the capability metadata printed on the serial console, followed by output showing that the derived memory region was successfully made accessible and written.
