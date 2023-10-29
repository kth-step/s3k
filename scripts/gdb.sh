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
layout split
fs cmd
EOF

riscv64-unknown-elf-gdb -x $TMP
rm $TMP
