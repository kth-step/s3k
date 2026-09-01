# Tutorial 05: Timing

This tutorial extends the multi-process setup with timing-slice capabilities. It shows how one process can derive or grant timing budget to another process and then run both under temporal partitioning.

Run it with:

```bash
./scripts/docker.sh
cd projects/tutorial.05.timing
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```
