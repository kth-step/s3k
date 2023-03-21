### Kernel configuration
# build dir
BUILD?=build
# config
CONFIG_H?=config.h
# Platform
PLATFORM?=plat/virt
# Prefix for toolchain
RISCV_PREFIX?=riscv64-unknown-elf-


### Compilation configuration
CC=${RISCV_PREFIX}gcc
OBJDUMP=${RISCV_PREFIX}objdump

INC =-include ${CONFIG_H} -Iinc -I${PLATFORM}
CFLAGS =-march=rv64imac -mabi=lp64 -mcmodel=medany\
	-std=gnu17\
	-Wall -Wextra -Werror\
	-Wno-unused-parameter\
	-Wshadow -fno-common\
	-ffunction-sections -fdata-sections\
	-ffreestanding\
	-Wno-builtin-declaration-mismatch\
	-flto -fwhole-program --specs=nosys.specs\
	-Os -g3
ASFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medany\
	-g3
LDFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medany\
	-nostartfiles -ffreestanding\
	-Wstack-usage=256 -fstack-usage\
	-Wl,--gc-sections\
	-flto\
	-T${PLATFORM}/linker.lds

#	-Wl,--no-warn-rwx-segments\ <== this option is not supported by all linkers
