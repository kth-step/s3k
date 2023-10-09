#!/bin/bash

KERNEL=build/s3k
APP0=build/app0
APP1=build/app1

if [[ -z "$TMUX" ]]; then
	echo "No tmux session"
	exit 0
fi

tmux split-window -d "./scripts/qemu.sh -s -S"
sleep 0.5

riscv64-unknown-elf-gdb				\
	-ex "set confirm off"			\
	-ex "set pagination off"		\
	-ex "set output-radix 16"		\
	-ex "symbol-file $KERNEL.elf"		\
	-ex "add-symbol-file $APP0.elf"		\
	-ex "j 0x80000000"			\
	-ex "b _hang"				\
	-ex "b *0x80010000"			\
	-ex "b *0x80020000"			\
	-ex "b handle_exception"		\
	-ex "target remote localhost:1234"	\
	-ex "layout split"			\
	-ex "fs cmd"

kill -SIGTERM $(pgrep qemu-system-*)
