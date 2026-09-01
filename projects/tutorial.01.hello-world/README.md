# Tutorial 01: Hello World

This tutorial shows the smallest S3K application. It enables UART access through the boot memory capability and prints a simple greeting.

Run it with:

```bash
./scripts/docker.sh
cd projects/tutorial.01.hello-world
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```
