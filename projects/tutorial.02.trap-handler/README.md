# Tutorial 02: Trap Handler

This tutorial installs a machine-mode trap handler, triggers an exception path, and prints the captured trap state such as `epc`, `esp`, `ecause`, and `eval`.

Run it with:

```bash
./scripts/docker.sh
cd projects/tutorial.02.trap-handler
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```
