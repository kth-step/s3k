### Kernel configuration
# Platform
PLATFORM?=virt


### Compilation configuration
# Prefix for toolchain
RISCV_PREFIX=riscv64-unknown-elf-
CC=${RISCV_PREFIX}gcc
OBJDUMP=${RISCV_PREFIX}objdump
CFLAGS = -march=rv64imac -mabi=lp64 -mcmodel=medany\
	 -std=gnu17\
	 -Wall -Wextra -Werror\
	 -Wno-unused-parameter\
	 -Wshadow -fno-common\
	 -ffunction-sections -fdata-sections\
	 -Wno-builtin-declaration-mismatch\
	 -flto -fwhole-program --specs=nosys.specs\
	 -Og -g3
ASFLAGS = -march=rv64imac -mabi=lp64 -mcmodel=medany
LDFLAGS = -march=rv64imac -mabi=lp64 -mcmodel=medany\
	  -nostartfiles -nostdlib\
	  -Wstack-usage=256 -fstack-usage\
	  -Wl,--gc-sections\
	  -flto\
	  -Tbsp/${PLATFORM}.lds
INC = -Iinc -Ibsp/${PLATFORM}/inc
