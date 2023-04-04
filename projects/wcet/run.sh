#!/bin/bash

KERNEL_ELF=s3k/s3k.elf
PAYLOAD_ELF=app0/app0.elf
PAYLOAD_BIN=app0/app0.bin

# Build the kernel and application
make -B

# Run the kernel, load the application as payload
qemu-system-riscv64 -M virt -smp 1 -m 128M -icount 3		\
	-nographic -bios none -kernel $KERNEL_ELF		\
	-device loader,file=$PAYLOAD_BIN,addr=0x80010000

# After qemu, kill linguring processes.
pkill -P $$
