#!/bin/bash

KERNEL=build/s3k
BOOT=build/boot

qemu-system-riscv64 -M sifive_u -smp 5 -m 128M		\
	-nographic -bios none -kernel $KERNEL.elf	\
	-semihosting \
	-device loader,file=$BOOT.bin,addr=0x80010000	\
	-serial tcp::8888,server,nowait \
	$@
