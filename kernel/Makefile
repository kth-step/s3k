# See LICENSE file for copyright and license details.
.POSIX:

# Kernel basename
ROOT=..
PROGRAM?=kernel

include ${ROOT}/tools.mk
include ${ROOT}/common/plat/${PLATFORM}.mk

# CC flags
CFLAGS:=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL} \
	-DPLATFORM_${PLATFORM} \
	-std=c11 \
	-Os -g3 \
	-Wall -Wextra -Werror \
	-Wno-unused-parameter \
	-Wshadow -fno-common \
	-Wno-builtin-declaration-mismatch \
	-fno-stack-protector \
	-flto \
	-include ${S3K_CONF_H} \
	-Iinc -I${COMMON_INC}

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
OBJS:=${patsubst src/%.S, ${BUILD}/${PROGRAM}/%.o, ${S_SRCS}} \
      ${patsubst src/%.c, ${BUILD}/${PROGRAM}/%.o, ${C_SRCS}}

# Dependency files
DEPS:=${OBJS:.o=.d}

# Targets
ELF:=${BUILD}/${PROGRAM}.elf
BIN:=${ELF:.elf=.bin}
HEX:=${ELF:.elf=.hex}
DA :=${ELF:.elf=.da}

all: ${ELF} ${BIN} ${HEX}

clean:
	rm -f ${ELF} ${OBJS} ${DEPS}

${BUILD}/${PROGRAM}/%.o: src/%.S
	@mkdir -p ${@D}
	${CC} -o $@ $< -c -MMD ${CFLAGS}

${BUILD}/${PROGRAM}/%.o: src/%.c
	@mkdir -p ${@D}
	${CC} -o $@ $< -c -MMD ${CFLAGS}

%.elf: ${OBJS}
	@mkdir -p ${@D}
	${CC} -o $@ ${OBJS} ${LDFLAGS}

%.bin: %.elf
	${OBJCOPY} -O binary $< $@

%.hex: %.elf
	${OBJCOPY} -O ihex $< $@

%.da: %.elf
	${OBJDUMP} -D $< > $@

.PHONY: all common da format clean

-include ${DEPS}
