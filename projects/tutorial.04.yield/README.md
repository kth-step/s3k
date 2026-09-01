# Tutorial 04: Yield

This tutorial starts a second process from the first one and uses monitor-mediated yielding to transfer execution between them.

Run it with:

```bash
./scripts/docker.sh
cd projects/tutorial.04.yield
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```
