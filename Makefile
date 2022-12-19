# See LICENSE file for copyright and license details.
.POSIX:

PROGRAM ?=s3k

BUILD?=build

SRC=src
INC=inc
GEN=gen

API=api

LDS        ?=config.lds
CONFIG_H   ?=config.h
BSP        ?=bsp/virt

SRCS=$(wildcard s3k/*.[cS])
OBJS=$(patsubst %, $(BUILD)/%.o, $(SRCS))

# Tools
RISCV_PREFIX ?=riscv64-unknown-elf-
CC=$(RISCV_PREFIX)gcc
LD=$(RISCV_PREFIX)ld
CPP=$(RISCV_PREFIX)cpp

CFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medany
CFLAGS+=-std=c18
CFLAGS+=-Wall
CFLAGS+=-gdwarf-2 -O2
CFLAGS+=-nostartfiles -static -ffreestanding
CFLAGS+=-Iinc -I$(BSP) -include $(CONFIG_H)
CFLAGS+=-T$(LDS)

.PHONY: all api clean
.SECONDARY:

all: $(BUILD)/s3k.elf

clean:
	rm -fr $(BUILD) inc/cap.h inc/offsets.h api/s3k.h

# Generated headers
inc/cap.h: gen/cap.yml
	gen/gen_cap $< $@

inc/offsets.h: gen/offsets.c inc/proc.h inc/consts.h
	$(CC) $(CFLAGS) -S -o $@ $< 
	sed -i -e '/#define/!d' -e 's/.\+#define/#define/' $@

$(BUILD)/%.c.o: %.c $(CONFIG_H) inc/cap.h inc/offsets.h
	@mkdir -p $(@D)
	$(CC) $(CFLAGS)    -c -o $@ $<

$(BUILD)/%.S.o: %.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS)    -c -o $@ $<

# Kernel
$(BUILD)/s3k.elf: $(LDS) $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS)    -o $@ $(OBJS)

# API
api: api/s3k.h
%s3k.h: api/s3k_call.h inc/cap.h inc/consts.h
	$(CPP) api/s3k_call.h > $@

