# See LICENSE file for copyright and license details.
.POSIX:

# Kernel basename
PROGRAM?=kernel

include ${ROOT}/tools.mk
include ${ROOT}/common/plat/${PLATFORM}.mk

# CC flags
CFLAGS=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL}
CFLAGS+=-DPLATFORM_${PLATFORM}
CFLAGS+=-std=c11
CFLAGS+=-Os -g3
CFLAGS+=-Wall -Wextra -Werror
CFLAGS+=-Wno-unused-parameter
CFLAGS+=-Wshadow -fno-common
CFLAGS+=-Wno-builtin-declaration-mismatch
CFLAGS+=-fno-stack-protector
CFLAGS+=-flto

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
DEPS=${OBJS:.o=.d}

# Targets
ELF=${BUILD}/${PROGRAM}.elf
BIN=${ELF:.elf=.bin}

all: ${ELF} ${BIN}
elf: ${ELF}
bin: ${BIN}

clean:
	rm -f ${ELF} ${OBJS} ${DEPS}

${BUILD}/${PROGRAM}/%.o: src/%.S
	@mkdir -p ${@D}
	@echo -e "CC\t${@F}"
	@${CC} -o $@ $< -c -MMD ${CFLAGS} 

${BUILD}/${PROGRAM}/%.o: src/%.c
	@mkdir -p ${@D}
	@echo -e "CC\t${@F}"
	@${CC} -o $@ $< -c -MMD ${CFLAGS}

%.elf: ${OBJS}
	@mkdir -p ${@D}
	@echo -e "CC\t${@F}"
	@${CC} -o $@ ${OBJS} ${LDFLAGS}

%.bin: %.elf
	@echo -e "OBJCOPY\t${@F}"
	@${OBJCOPY} -O binary $< $@

.PHONY: all common elf bin da format clean

-include ${DEPS}
