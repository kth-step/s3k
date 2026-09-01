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

After the QEMU run finishes, read back the generated file from the disk image with:

```bash
ninja -C builddir disk-read
```

This prints the directory contents and then extracts `NEWFILE.TXT` so you can verify that the tutorial wrote the file successfully.
