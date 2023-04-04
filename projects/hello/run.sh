#!/bin/bash

KERNEL=build/s3k
APP0=build/app0
APP1=build/app1

# Build the kernel and application
make all

# Run the kernel, load the application as payload
qemu-system-riscv64 -M virt -smp 1 -m 128M			\
	-nographic -bios none -kernel $KERNEL.elf		\
	-device loader,file=$APP0.bin,addr=0x80010000	\
	-device loader,file=$APP1.bin,addr=0x80020000	\
	-s -S &

st -e	riscv64-unknown-elf-gdb				\
	-ex "set confirm off"				\
	-ex "set pagination off"			\
	-ex "set output-radix 16"			\
	-ex "symbol-file $KERNEL.elf"			\
	-ex "add-symbol-file $APP0.elf"	\
	-ex "add-symbol-file $APP1.elf"	\
	-ex "j 0x80000000"				\
	-ex "b _hang"					\
	-ex "b *0x80010000"				\
	-ex "b *0x80020000"				\
	-ex "b handle_exception"			\
	-ex "target remote localhost:1234"		\
	-ex "layout split"				\
	-ex "fs cmd"

# After qemu, kill linguring processes.
pkill -P $$
