# See LICENSE file for copyright and license details.
.POSIX:

PROGRAM ?=s3k

SRC=src
INC=inc
GEN=gen

API=api

LDS        ?=config.lds
CONFIG_H   ?=config.h
BSP        ?=bsp/virt

SRCS=$(wildcard src/*.[cS]) $(wildcard *.S)
GHDRS=inc/cap.g.h inc/offsets.g.h 
HDRS=$(wildcard inc/*.h) $(CONFIG_H) $(PLATFORM_H)

CAP_H=cap.g.h
ASM_CONSTS_H=asm_consts.g.h

# Tools
RISCV_PREFIX ?=riscv64-unknown-elf-
CC=$(RISCV_PREFIX)gcc
LD=$(RISCV_PREFIX)ld

CFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medany
CFLAGS+=-std=c18
CFLAGS+=-Wall
CFLAGS+=-gdwarf-2 -O2
CFLAGS+=-nostartfiles -static -ffreestanding
CFLAGS+=-Iinc -I$(BSP) -include $(CONFIG_H) 
CFLAGS+=-T$(LDS)
CFLAGS+=-DNDEBUG

.PHONY: all api clean
.SECONDARY:

all: build/s3k.elf

clean:
	rm -f inc/*.g.h api/*.g.h build/s3k.elf

# Generated headers
inc/cap.g.h: gen/cap.yml
	gen/gen_cap $< $@

inc/offsets.g.h: gen/offsets.c inc/proc.h inc/consts.h
	$(CC) $(CFLAGS) -S -o $@ $< 
	sed -i -e '/#define/!d' -e 's/.\+#define/#define/' $@

# Kernel
%.elf: $(LDS) $(SRCS) $(HDRS) $(GHDRS) $(PAYLOAD)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

# API
api: api/s3k_cap.g.h api/s3k_consts.g.h

api/s3k_cap.g.h: inc/cap.g.h
	sed '/kassert/d' $< > $@

api/s3k_consts.g.h: inc/consts.h
	cp $< $@
