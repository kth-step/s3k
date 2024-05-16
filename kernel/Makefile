# See LICENSE file for copyright and license details.
.POSIX:
.SECONDARY:

# Kernel basename
ROOT=..
PROGRAM?=kernel

include ${ROOT}/tools.mk
include ${ROOT}/common/plat/${PLATFORM}.mk

# CC flags
CFLAGS:=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL} \
	-DPLATFORM_${PLATFORM} \
	-std=c11 \
	-O2 -g3 \
	-Wall -Wextra -Werror \
	-Wno-unused-parameter \
	-Wshadow -fno-common \
	-Wno-builtin-declaration-mismatch \
	-fno-stack-protector \
	-flto -fwhole-program \
	-include ${S3K_CONF_H} \
	-Iinc -I${COMMON_INC} \

# LD flags
LDFLAGS:=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL} \
	 -Tlinker.ld \
	 -nostartfiles -ffreestanding \
	 -flto -fwhole-program \
	 -Wl,--no-warn-rwx-segment \
	 -Wl,--gc-sections \
	 --specs=nosys.specs \
	 -ffunction-sections -fdata-sections \
	 -L${COMMON_LIB} \
	 -laltc -lplat

# Source files
S_SRCS:=${wildcard src/*.S}
C_SRCS:=${wildcard src/*.c}

# Object files
OBJS:=${patsubst src/%.S, ${BUILD}/${PROGRAM}/%.S.o, ${S_SRCS}} \
      ${patsubst src/%.c, ${BUILD}/${PROGRAM}/%.c.o, ${C_SRCS}}

# Dependency files
DEPS:=${OBJS:.o=.d}

# Targets
ELF:=${BUILD}/${PROGRAM}.elf
BIN:=${ELF:.elf=.bin}
HEX:=${ELF:.elf=.hex}
DA :=${ELF:.elf=.da}

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

%.da: %.elf
	@${OBJDUMP} -D $< > $@
	@printf "OBJDUMP\t$@\n"

.PHONY: all da format clean

-include ${DEPS}
