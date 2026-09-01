# Tutorial 03: Memory Capabilities

This tutorial demonstrates how to inspect a boot memory capability, derive a new memory capability from it, program a PMP entry, and write through the newly authorized memory region.

Run it with:

```bash
./scripts/docker.sh
cd projects/tutorial.03.mem-cap
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```
