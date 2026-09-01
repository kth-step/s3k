# Tutorial 08: IPC With Capability Transfer

This tutorial extends IPC with capability transfer. The client derives a memory capability, sends it to the server through IPC, and the server maps and reads the shared memory region before replying.

Run it with:

```bash
./scripts/docker.sh
cd projects/tutorial.08.ipc-cap
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```
