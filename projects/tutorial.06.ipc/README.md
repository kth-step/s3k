# Tutorial 06: IPC

This tutorial sets up a client and server process, derives synchronous IPC endpoints, grants the server endpoint to the second process, and exchanges messages through `call` and `replyrecv`.

Run it with:

```bash
./scripts/docker.sh
cd projects/tutorial.06.ipc
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```
