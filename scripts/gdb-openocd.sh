#!/usr/bin/env bash

TMP=`mktemp`

cat << EOF > $TMP
set confirm off
set pagination off
set output-radix 16
`
for elf in $ELFS; do
	echo "add-symbol-file $elf"
done
`
target extended-remote :3333
monitor reset init
continue
`
for elf in $ELFS; do
	hex=${elf%.elf}.hex
	printf "load $hex\n"
done
`
thread apply all set \$pc=0x80000000
layout split
fs cmd
EOF

riscv64-unknown-elf-gdb -x $TMP
rm $TMP
