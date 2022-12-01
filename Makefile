# See LICENSE file for copyright and license details.
.POSIX:

PROGRAM ?=s3k

SRC=src
INC=inc
GEN=gen
SCRIPTS=scripts

ELF=$(PROGRAM).elf
BIN=$(PROGRAM).bin
DA=$(PROGRAM).da
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
RISCV_PREFIX ?=riscv64-unknown-elf
CC=$(RISCV_PREFIX)-gcc
LD=$(RISCV_PREFIX)-ld
SIZE=$(RISCV_PREFIX)-size
OBJCOPY=$(RISCV_PREFIX)-objcopy
OBJDUMP=$(RISCV_PREFIX)-objdump

CFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medany
CFLAGS+=-std=c18
CFLAGS+=-Wall
CFLAGS+=-gdwarf-2 -O3
CFLAGS+=-nostdlib -static -ffreestanding
CFLAGS+=-Iinc -I$(BSP) -include $(CONFIG_H) 
CFLAGS+=-T$(LDS)

.PHONY: all api clean
.SECONDARY:

all: $(ELF) $(BIN) $(DA)

clean:
	rm -f inc/*.g.h api/*.g.h $(ELF) $(BIN) $(DA)

# Generated headers
inc/cap.g.h: gen/cap.yml
	gen/gen_cap $< $@

inc/offsets.g.h: gen/offsets.c inc/proc.h inc/consts.h
	$(CC) $(CFLAGS) -S -o $@ $< 
	sed -i -e '/#define/!d' -e 's/.\+#define/#define/' $@

# Kernel
$(ELF): $(LDS) $(SRCS) $(HDRS) $(GHDRS) $(PAYLOAD)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(DA): $(ELF)
	$(OBJDUMP) -d $< > $@

# API
api: api/s3k.h

api/s3k.h: api/s3k_cap.g.h api/s3k_consts.g.h
	touch $@

api/s3k_cap.g.h: inc/cap.g.h
	sed '/kassert/d' $< > $@

api/s3k_consts.g.h: inc/consts.h
	cp $< $@
