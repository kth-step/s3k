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

SRCS=$(wildcard $(SRC)/*.c) $(wildcard $(SRC)/*.S)
HDRS=$(wildcard $(INC)/*.h) $(CAP_H) $(ASM_CONSTS_H) $(CONFIG_H) $(PLATFORM_H)

CAP_H=$(INC)/cap.h
ASM_CONSTS_H=$(INC)/asm_consts.h

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
CFLAGS+=-I$(INC) -I$(BSP) -include $(BSP).h -include $(CONFIG_H) 
CFLAGS+=-T$(LDS)

.PHONY: all target clean size da cloc format elf bin da api
.SECONDARY:

all: $(ELF) $(BIN) $(DA)

elf: $(ELF)
bin: $(BIN)
da: $(DA)


clean:
	rm -f $(CAP_H) $(ASM_CONST_H) $(ELF) $(BIN) $(DA)

size:
	$(SIZE) $(ELF)

cloc:
	cloc $(HDRS) $(SRCS)

format:
	clang-format -i $(wildcard inc/*.h) $(wildcard src/*.c) $(wildcard api/*.h)

# Generated headers
$(CAP_H): $(GEN)/cap.yml
	$(SCRIPTS)/gen_cap $< $@

$(ASM_CONSTS_H): $(GEN)/asm_consts.c $(INC)/proc.h $(INC)/cap_node.h $(INC)/consts.h
	$(CC) -include $(CONFIG_H) -I$(INC) -I$(BSP) -S -o $@ $< 
	sed -i -e '/#define/!d' -e 's/.\+#define/#define/' $@

# Kernel
$(ELF): $(LDS) $(SRCS) $(HDRS) $(PAYLOAD)
	$(CC) $(CFLAGS) -o $@ $(SRCS) -DPAYLOAD=\"$(PAYLOAD)\"

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

%.da: %.elf
	$(OBJDUMP) -d $< > $@

# API
api: $(API)/s3k_consts.h $(API)/s3k_cap.h

$(API)/s3k_cap.h: $(INC)/cap.h
	cp $< $@
	sed -i '/kassert/d' $@

$(API)/s3k_consts.h: $(INC)/consts.h
	cp $< $@
