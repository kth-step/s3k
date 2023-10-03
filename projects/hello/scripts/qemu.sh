#!/bin/bash

KERNEL=build/s3k
APP0=build/app0
APP1=build/app1

qemu-system-riscv64 -M virt -smp 2 -m 128M		\
	-nographic -bios none -kernel $KERNEL.elf	\
	-device loader,file=$APP0.bin,addr=0x80010000	\
	-device loader,file=$APP1.bin,addr=0x80020000	\
	$@
