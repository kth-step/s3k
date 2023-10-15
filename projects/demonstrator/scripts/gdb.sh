#!/bin/bash

KERNEL=build/s3k
BOOT=build/boot
MONITOR=build/monitor
CRYPTO=build/crypto
UART=build/uart

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
	-ex "add-symbol-file $BOOT.elf"		\
	-ex "add-symbol-file $MONITOR.elf"	\
	-ex "add-symbol-file $CRYPTO.elf"	\
	-ex "add-symbol-file $UART.elf"		\
	-ex "b _hang"				\
	-ex "b handle_exception"		\
	-ex "b *0x80010000"			\
	-ex "target extended-remote :1234"	\
	-ex "add-inferior" \
	-ex "inferior 2" \
	-ex "attach 2" \
	-ex "symbol-file $KERNEL.elf"\
	-ex "layout split"			\
	-ex "fs cmd"

kill -SIGTERM $(pgrep qemu-system-*)
