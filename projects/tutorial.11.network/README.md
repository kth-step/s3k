# Tutorial 11: Networking

This tutorial brings up a VirtIO network device and runs a tiny telnet-based integer service on guest port `23`. Sending an integer followed by a newline returns that integer plus one.

Run it with:

```bash
./scripts/docker.sh
cd projects/tutorial.11.network
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```

With QEMU running, connect from macOS or Linux on the host:

```bash
telnet 127.0.0.1 5555
```

Example request:

```text
45
```

Expected response:

```text
46
```
