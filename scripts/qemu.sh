#!/usr/bin/env bash


LOADER=""
for elf in $ELFS; do
	LOADER+="-device loader,file=$elf "
done

if [[ -z "$QEMU_MACHINE" ]]; then
	echo "QEMU_MACHINE not set"
fi

if [[ -z "$QEMU_SMP" ]]; then
	echo "QEMU_SMP not set"
fi

qemu-system-riscv64 -M $QEMU_MACHINE -smp $QEMU_SMP -m 128M -nographic -bios none \
	$LOADER $QEMU_OPTIONS $@
