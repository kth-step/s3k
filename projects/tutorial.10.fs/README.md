# Tutorial 10: Filesystem

This tutorial brings up a VirtIO block device and a FAT filesystem. The application mounts the disk image, creates `newfile.txt`, and reads the preloaded `test.txt`.

Create the disk image before running QEMU:

```bash
./scripts/docker.sh
cd projects/tutorial.10.fs
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir disk-image
ninja -C builddir qemu-run
```

To inspect the resulting image from inside the container after the run:

```bash
ninja -C builddir disk-read
```
