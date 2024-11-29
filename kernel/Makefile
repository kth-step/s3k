# See LICENSE file for copyright and license details.
.POSIX:
.SECONDARY:

# Kernel basename
ROOT=..
PROGRAM?=kernel
PLATFORM?=qemu_virt4
BUILD?=build/${PLATFORM}
S3K_CONF_H?=s3k_conf.h

include ${ROOT}/tools.mk
include ${ROOT}/common/plat/${PLATFORM}.mk

# CC flags
CFLAGS:=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL} \
	-DPLATFORM_${PLATFORM} \
	-std=c11 \
	-O2 -g3 \
	-flto -ffat-lto-objects \
	-Wall -Wextra -Werror \
	-Wno-unused-parameter \
	-Wshadow -fno-common \
	-fno-stack-protector \
	-include ${S3K_CONF_H} \
	-Iinc -I${COMMON_INC} \

# LD flags
LDFLAGS:=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL} \
	 -Tlinker.ld \
	 -nostartfiles -ffreestanding \
	 -Wl,--no-warn-rwx-segment \
	 -Wl,--gc-sections \
	 -flto \
	 --specs=nosys.specs \
	 -ffunction-sections -fdata-sections \
	 -L${COMMON_LIB} \
	 -laltc -lplat

# Source files
S_SRCS:=${wildcard src/*.S}
C_SRCS:=${wildcard src/*.c} ${wildcard src/*/*.c}

# Object files
OBJS:=${patsubst src/%.S, ${BUILD}/${PROGRAM}/%.S.o, ${S_SRCS}} \
      ${patsubst src/%.c, ${BUILD}/${PROGRAM}/%.c.o, ${C_SRCS}}

# Dependency files
DEPS:=${OBJS:.o=.d}

# Targets
ELF:=${BUILD}/${PROGRAM}.elf
BIN:=${ELF:.elf=.bin}
HEX:=${ELF:.elf=.hex}
DA :=${ELF:.elf=.da} ${patsubst %.o, %.da, ${OBJS}}


all: ${ELF} ${BIN} ${HEX} ${DA}

clean:
	rm -f ${ELF} ${OBJS} ${DEPS}

${BUILD}/${PROGRAM}/%.S.o: src/%.S
	@mkdir -p ${@D}
	@${CC} -o $@ $< -c -MMD ${CFLAGS}
	@printf "CC\t$@\n"

${BUILD}/${PROGRAM}/%.c.o: src/%.c
	@mkdir -p ${@D}
	@${CC} -o $@ $< -c -MMD ${CFLAGS}
	@printf "CC\t$@\n"

%.elf: ${OBJS}
	@mkdir -p ${@D}
	@${CC} -o $@ ${OBJS} ${LDFLAGS}
	@printf "CC\t$@\n"

%.bin: %.elf
	@${OBJCOPY} -O binary $< $@
	@printf "OBJCOPY\t$@\n"

%.hex: %.elf
	@${OBJCOPY} -O ihex $< $@
	@printf "OBJCOPY\t$@\n"

%.c.da: %.c.o
	@${OBJDUMP} -d $< > $@
	@printf "OBJDUMP\t$@\n"

%.S.da: %.S.o
	@${OBJDUMP} -d $< > $@
	@printf "OBJDUMP\t$@\n"

%.da: %.elf
	@${OBJDUMP} -d $< > $@
	@printf "OBJDUMP\t$@\n"

.PHONY: all da format clean

-include ${DEPS}
