.POSIX:

BUILD?=build
PROGRAM?=a

include ${ROOT}/tools.mk
include ${ROOT}/common/plat/${PLATFORM}.mk

C_SRCS=${wildcard ${PROGRAM}/*.c}
S_SRCS=${wildcard ${PROGRAM}/*.S}
OBJS=${patsubst %.c, ${BUILD}/%.o, ${C_SRCS}} \
     ${patsubst %.S, ${BUILD}/%.o, ${S_SRCS}} \
     ${STARTFILES}/start.o
DEPS=${OBJS:.o=.d}

CFLAGS=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL}
CFLAGS+=-DPLATFORM_${PLATFORM}
CFLAGS+=-nostdlib
CFLAGS+=-DSTACK_SIZE=1024
CFLAGS+=-Os -g3 -flto
CFLAGS+=-I${COMMON_INC} -include ${S3K_CONF_H}

LDFLAGS=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL}
LDFLAGS+=-nostdlib
LDFLAGS+=-flto
LDFLAGS+=-T${PROGRAM}.ld -Tdefault.ld
LDFLAGS+=-Wl,--no-warn-rwx-segments
LDFLAGS+=-L${COMMON_LIB} -ls3k -laltc -lplat

ELF=${BUILD}/${PROGRAM}.elf
BIN=${ELF:.elf=.bin}

all: ${ELF} ${BIN}

clean:
	rm -f ${ELF} ${OBJS} ${DEPS}

${BUILD}/${PROGRAM}/%.o: ${PROGRAM}/%.S
	@mkdir -p ${@D}
	${CC} -o $@ $< ${CFLAGS} ${INC} -MMD -c

${BUILD}/${PROGRAM}/%.o: ${PROGRAM}/%.c
	@mkdir -p ${@D}
	${CC} -o $@ $< ${CFLAGS} ${INC} -MMD -c

%.elf: ${OBJS}
	@mkdir -p ${@D}
	${CC} -o $@ ${OBJS} ${LDFLAGS} ${INC}

%.bin: %.elf
	${OBJCOPY} -O binary $< $@

.PHONY: all elf clean

-include ${DEPS}

