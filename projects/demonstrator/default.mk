BUILD?=build
PROGRAM?=a

ROOT=../../..
USERLAND=${ROOT}/userland
PLATFORM=${ROOT}/plat64

include ${ROOT}/tools.mk
include ${PLATFORM}/config.mk

vpath %.c . ${USERLAND}/src
vpath %.S . ${USERLAND}/src

OBJS=${patsubst %, ${BUILD}/${PROGRAM}/%.o, ${SRCS}}
DEPS=${patsubst %, ${BUILD}/${PROGRAM}/%.d, ${SRCS}}

CFLAGS+=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL}
CFLAGS+=-nostartfiles -nostdlib
CFLAGS+=-DSTACK_SIZE=1024
CFLAGS+=-Tlinker.ld -T../default.ld
CFLAGS+=-Wl,--no-warn-rwx-segments

INC=-Iinc -I${PLATFORM}/inc -I${USERLAND}/inc -include ${S3K_CONF_H}

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
	${CC} ${CFLAGS} -MMD ${INC} -o $@ ${OBJS}

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
