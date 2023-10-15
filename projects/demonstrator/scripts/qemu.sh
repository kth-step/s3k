#!/bin/bash

KERNEL=build/s3k
BOOT=build/boot

qemu-system-riscv64 -M virt -icount 1 -smp 1 -m 128M		\
	-nographic -bios none -kernel $KERNEL.elf	\
	-semihosting \
	-device loader,file=$BOOT.bin,addr=0x80010000	\
	-serial tcp:127.0.0.1:4321,server,nowait \
	$@
