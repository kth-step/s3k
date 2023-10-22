# See LICENSE file for copyright and license details.
.POSIX:

# Kernel basename
PROGRAM?=s3k

# Kernel config
S3K_CONF_H?=s3k_conf.h

# Platform config
PLATFORM?=qemu_virt

# Repository root
ROOT=..

# Build target
BUILD?=build

# Get toolchain
include ${ROOT}/tools.mk

# Platform specific config
include ${ROOT}/common/plat/${PLATFORM}.mk

# CC flags
CFLAGS=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL}
CFLAGS+=-std=c11
CFLAGS+=-Os -g3
CFLAGS+=-Wall -Wextra -Werror
CFLAGS+=-Wno-unused-parameter
CFLAGS+=-Wshadow -fno-common
CFLAGS+=-Wno-builtin-declaration-mismatch
CFLAGS+=-fno-stack-protector
CFLAGS+=-flto
CFLAGS+=-DPLATFORM_${PLATFORM}

# Include files and directories
CFLAGS+=-include ${S3K_CONF_H}
CFLAGS+=-Iinc -I${COMMON_INC}

# LD flags
LDFLAGS=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL}
LDFLAGS+=-Tlinker.ld
LDFLAGS+=-nostartfiles -ffreestanding
LDFLAGS+=-flto -fwhole-program
LDFLAGS+=-Wl,--no-warn-rwx-segment
LDFLAGS+=-Wl,--gc-sections
LDFLAGS+=--specs=nosys.specs
LDFLAGS+=-ffunction-sections -fdata-sections
LDFLAGS+=-L${COMMON_LIB}
LDFLAGS+=-laltc -lplat

# Source files
S_SRCS=${wildcard src/*.S}
C_SRCS=${wildcard src/*.c}
 
# Object files
OBJS=${patsubst src/%.S, ${BUILD}/${PROGRAM}/%.o, ${S_SRCS}} \
     ${patsubst src/%.c, ${BUILD}/${PROGRAM}/%.o, ${C_SRCS}}

# Dependency files
DEPS=${patsubst %.o, ${BUILD}/obj/%.d, ${OBJS}}

# Targets
ELF=${BUILD}/${PROGRAM}.elf
BIN=${ELF:.elf=.bin}
DA=${ELF:.elf=.da}

all: ${ELF} ${BIN} ${DA}

elf: ${ELF}
bin: ${BIN}
da: ${DA}

${BUILD}/${PROGRAM}/%.o: src/%.S
	@mkdir -p ${@D}
	${CC} -o $@ $< -c -MMD ${CFLAGS} 

${BUILD}/${PROGRAM}/%.o: src/%.c
	@mkdir -p ${@D}
	${CC} -o $@ $< -c -MMD ${CFLAGS}

${ELF}: ${OBJS}
	@mkdir -p ${@D}
	${CC} -o $@ ${OBJS} ${LDFLAGS} 

${BIN}: ${ELF}
	${OBJCOPY} -O binary $< $@

${DA}: ${ELF}
	${OBJDUMP} -D $< > $@

format:
	clang-format -i $(shell find -name '*.[hc]' -not -path '*/.*')

clean:
	rm -fr ${BUILD}

.PHONY: all elf bin da format clean

-include ${DEPS}
