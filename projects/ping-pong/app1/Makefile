BUILD?=build
PROGRAM?=app1

ROOT=../../..
COMMON=${ROOT}/common
PLATFORM=${ROOT}/plat/virt

include ${ROOT}/tools.mk
include ${PLATFORM}/platform.mk

vpath %.c src ${COMMON}/src
vpath %.S src ${COMMON}/src

SRCS=init/start.S altio/altio.c s3k/s3k.c
SRCS+=${DRIVERS}
SRCS+=main.c

OBJS=${patsubst %, ${BUILD}/${PROGRAM}/%.o, ${SRCS}}
DEPS=${patsubst %, ${BUILD}/${PROGRAM}/%.d, ${SRCS}}

CFLAGS+=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL}
CFLAGS+=-nostartfiles -nostdlib
CFLAGS+=-DSTACK_SIZE=1024
CFLAGS+=-T${PLATFORM}/platform.ld -Tlinker.ld
CFLAGS+=-Wl,--no-warn-rwx-segments

INC=-Iinc -I${COMMON}/inc -include ${S3K_CONF_H}

ELF=${BUILD}/${PROGRAM}.elf
BIN=${ELF:.elf=.bin}
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
	@${CC} ${CFLAGS} -MMD ${INC} -o $@ ${OBJS}

${BIN}: ${ELF}
	@echo "OBJCOPY $@"
	@${OBJCOPY} -O binary $< $@

${DA}: ${ELF}
	@echo "OBJDUMP $@"
	@${OBJDUMP} -D $< > $@

clean:
	rm -f ${ELF} ${BIN} ${DA} ${OBJS} ${DEPS}

.PHONY: all elf bin da clean

-include ${DEPS}
