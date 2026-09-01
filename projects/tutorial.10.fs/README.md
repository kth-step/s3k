# Tutorial 10: Filesystem

## What this tutorial is about

This tutorial introduces storage I/O. The application enables access to a VirtIO block device, mounts a FAT filesystem, creates `newfile.txt`, and reads the existing `test.txt` from the disk image.

## Lecture Notes

One recurring theme in S3K is that common operating-system services are not built into the kernel by default. Filesystems are a good example. The kernel provides isolation and resource control; filesystem logic lives in user space and depends on explicit access to the underlying device.

This tutorial demonstrates that layering:

- the process gets explicit MMIO access to the UART and VirtIO block device,
- the block device is used by a filesystem implementation in user space,
- the application performs ordinary file operations on top of that stack.

That is important pedagogically. The tutorial is not saying "S3K has no filesystems." It is showing that filesystems are built as services on top of the kernel's resource model rather than being inseparable from the kernel itself.

## What to look at in the code

Read [app1/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.10.fs/app1/main.c). Focus on:

- the capability setup for UART and VirtIO,
- the filesystem mount path,
- file creation and file readback.

Also inspect the Meson targets in this tutorial: the disk image is prepared before running QEMU, and a helper target lets you read the image afterward from the host side.

## How to run it

Create the disk image, boot the guest, and then inspect the modified image afterward:

```bash
./scripts/docker.sh
cd projects/tutorial.10.fs
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir disk-image
ninja -C builddir qemu-run
```

After the QEMU run finishes, read back the generated file from the image:

```bash
ninja -C builddir disk-read
```

## Expected result

During QEMU execution, the guest should mount the FAT image, create `newfile.txt`, and read `test.txt`. The `disk-read` target should then list the image contents and extract `NEWFILE.TXT` so you can confirm that the file was really written.
