#!/usr/bin/env sh

# Input files
file1="build/qemu_esp32c3/kernel.bin"
file2="build/qemu_esp32c3/app0.bin"
output="build/qemu_esp32c3/merged.bin"

# Base address for the first file
base1=0x0

# Get size of the first file
size1=$(stat -f%z  "$file1")

# Calculate next aligned address
start2=65536

# Generate padding
padding_size=$((start2 - (size1 + base1)))
dd if=/dev/zero bs=1 count=$padding_size > build/qemu_esp32c3/padding.bin

# Merge files
cat "$file1" build/qemu_esp32c3/padding.bin "$file2" > "$output"

# Cleanup
rm build/qemu_esp32c3/padding.bin

echo "Merged $file1 and $file2 into $output starting at $base1 and $start2"
