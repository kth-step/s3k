# See LICENSE file for copyright and license details.
.POSIX:

# Program name
BUILD?=build
PROGRAM?=s3k

# Root of repository
ROOT=..

# Kernel configuration
S3K_CONF_H?=s3k_conf.h

# Platform specific config
PLATFORM?=${ROOT}/plat/virt
include ${PLATFORM}/platform.mk

# Common sources
COMMON=${ROOT}/common

# Get toolchain
include ${ROOT}/tools.mk

# Search path
vpath %.c src ${COMMON}/src
vpath %.S src

# Source files
SRCS =head.S trap.S stack.S
SRCS+=cap_table.c cap_types.c cap_ops.c cap_pmp.c cap_monitor.c cap_ipc.c \
      csr.c exception.c kernel.c proc.c sched.c mcslock.c syscall.c
SRCS+=${DRIVERS}
OBJS=${patsubst %, ${BUILD}/${PROGRAM}/%.o, ${SRCS}}
DEPS=${patsubst %, ${BUILD}/${PROGRAM}/%.d, ${SRCS}}

# Compilation flags
CFLAGS =-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL}
CFLAGS+=-std=c11
CFLAGS+=-Wall -Wextra -Werror
CFLAGS+=-Wno-unused-parameter
CFLAGS+=-Wshadow -fno-common
CFLAGS+=-Wno-builtin-declaration-mismatch
CFLAGS+=-Os -g3
CFLAGS+=-nostartfiles -ffreestanding
CFLAGS+=-fno-stack-protector
CFLAGS+=-Wl,--gc-sections
CFLAGS+=-flto -fwhole-program
CFLAGS+=--specs=nosys.specs
CFLAGS+=-ffunction-sections -fdata-sections
CFLAGS+=-T${PLATFORM}/platform.ld -Tlinker.ld
CFLAGS+=-Wl,--no-warn-rwx-segments

INC+=-include ${PLAT_CONF_H} -include ${S3K_CONF_H}
INC+=-Iinc -I${COMMON}/inc

ELF=${BUILD}/${PROGRAM}.elf
BIN=${ELF:.elf=.bin}
DEP=${ELF:.elf=.d}
DA=${ELF:.elf=.da}

all: ${ELF} ${BIN} ${DA}

elf: ${ELF}
bin: ${BIN}
da: ${DA}

${BUILD}/${PROGRAM}/%.S.o: %.S
	@mkdir -p ${@D}
	@echo "CC $@"
	@${CC} ${CFLAGS} ${INC} -MMD -c -o $@ $<

${BUILD}/${PROGRAM}/%.c.o: %.c
	@mkdir -p ${@D}
	@echo "CC $@"
	@${CC} ${CFLAGS} ${INC} -MMD -c -o $@ $<

${ELF}: ${OBJS}
	@mkdir -p ${@D}
	@echo "CC $@"
	@${CC} ${CFLAGS} ${INC} -o $@ ${OBJS}

${BIN}: ${ELF}
	@echo "OBJCOPY $@"
	@${OBJCOPY} -O binary $< $@

${DA}: ${ELF}
	@echo "OBJDUMP $@"
	@${OBJDUMP} -D $< > $@

format:
	clang-format -i $(shell find -name '*.[hc]' -not -path '*/.*')

clean:
	rm -f ${ELF} ${BIN} ${DA} ${OBJS} ${DEPS}

.PHONY: all elf bin da format clean

-include ${DEPS}
