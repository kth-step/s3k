RISCV_PREFIX?=riscv64-unknown-elf-
export CC     :=${RISCV_PREFIX}gcc
export AR     :=${RISCV_PREFIX}ar
export LD     :=${RISCV_PREFIX}ld
export SIZE   :=${RISCV_PREFIX}size
export OBJDUMP:=${RISCV_PREFIX}objdump
export OBJCOPY:=${RISCV_PREFIX}objcopy
export GDB    :=${RISCV_PREFIX}gdb
