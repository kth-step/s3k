# See LICENSE file for copyright and license details.
.POSIX:
.PHONY: all api clean
.SECONDARY:

BUILD?=build

CONFIG_H ?=config.h
BSP      ?=bsp/virt

# Tools
RISCV_PREFIX ?=riscv64-unknown-elf-
CC=$(RISCV_PREFIX)gcc
LD=$(RISCV_PREFIX)ld

# Compilation flags
CFLAGS =-march=rv64imac -mabi=lp64 -mcmodel=medany
CFLAGS+=-std=gnu18
CFLAGS+=-Wall -Werror
CFLAGS+=-g -O2
CFLAGS+=-static
CFLAGS+=-nostartfiles -ffreestanding
CFLAGS+=-Iinc -I$(BSP) 
CFLAGS+=-include $(CONFIG_H)
CFLAGS+=-Ts3k.lds
CFLAGS+=-DNDEBUG

# Source Files
C_SRC=$(wildcard s3k/*.c)
S_SRC=$(wildcard s3k/*.S)

# Target objects
OBJ=$(patsubst %.c, $(BUILD)/%.o, $(C_SRC)) $(patsubst %.S, $(BUILD)/%.o, $(S_SRC))


all: $(BUILD)/s3k.elf

clean:
	rm -fr $(BUILD) inc/cap.h inc/offsets.h api/s3k.h


# Generated capability header
inc/cap.h: gen/cap.yml
	./scripts/gen_cap.py $< $@


# Generated offset header
inc/offsets.h: gen/offsets.c inc/proc.h inc/consts.h
	$(CC) $(CFLAGS) -S -o $@ $< 
	sed -i -e '/#define/!d' -e 's/.\+#define/#define/' $@


# Compile C files
$(BUILD)/%.o: %.c $(CONFIG_H) inc/cap.h inc/offsets.h
	@mkdir -p $(@D)
	$(CC) $(CFLAGS)    -c -o $@ $<

# Compile ASM files
$(BUILD)/%.o: %.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS)    -c -o $@ $<

# Kernel
$(BUILD)/s3k.elf: $(LDS) $(OBJ)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS)    -o $@ $(OBJ)

# API
api: api/s3k.h
%s3k.h: api/s3k_call.h inc/cap.h inc/consts.h
	$(CPP) api/s3k_call.h > $@

