BUILD?=build
PROGRAM?=a

ROOT=../../..
STARTFILES=${ROOT}/common/start

include ${ROOT}/tools.mk
include ${ROOT}/common/plat/${PLATFORM}.mk

OBJS=${patsubst %, ${BUILD}/${PROGRAM}/%.o, ${SRCS}} ${STARTFILES}/start.o
DEPS=${patsubst %, ${BUILD}/${PROGRAM}/%.d, ${SRCS}}

CFLAGS=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL}
CFLAGS+=-DPLATFORM_${PLATFORM}
CFLAGS+=-nostdlib
CFLAGS+=-DSTACK_SIZE=1024
CFLAGS+=-Os -g3
CFLAGS+=-Iinc -I${COMMON_INC} -include ${S3K_CONF_H}

LDFLAGS=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL}
LDFLAGS+=-g3
LDFLAGS+=-nostartfiles -nostdlib
LDFLAGS+=-Tlinker.ld
LDFLAGS+=-Wl,--no-warn-rwx-segments
LDFLAGS+=-L${COMMON_LIB} -ls3k -laltc -lplat


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
	@${CC} -o $@ $< ${CFLAGS} ${INC} -MMD -c
${BUILD}/${PROGRAM}/%.c.o: %.c
	@mkdir -p ${@D}
	@echo "CC $@"
	@${CC} -o $@ $< ${CFLAGS} ${INC} -MMD -c

${ELF}: ${OBJS}
	@mkdir -p ${@D}
	@echo "CC $@"
	@${CC} -o $@ ${OBJS} ${LDFLAGS} -MMD ${INC}

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
