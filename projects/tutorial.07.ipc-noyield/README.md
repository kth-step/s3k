# Tutorial 07: IPC Without Yield

This tutorial explores asynchronous IPC without the yield flag. It sets up a server in a second process and uses `replyrecv` on both sides to exchange messages under a different scheduling pattern than tutorial 06.

Run it with:

```bash
./scripts/docker.sh
cd projects/tutorial.07.ipc-noyield
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```
