# Tutorial 09: Three Processes

This tutorial boots and manages three processes. The first process dynamically redistributes timing budget between the other two so you can observe changing execution shares at runtime.

Run it with:

```bash
./scripts/docker.sh
cd projects/tutorial.09.3.processes
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```
