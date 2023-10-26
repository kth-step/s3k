BUILD?=build
PROGRAM?=a

include ${ROOT}/tools.mk
include ${ROOT}/common/plat/${PLATFORM}.mk

SRCS=${wildcard ${PROGRAM}/*.[cS]}
OBJS=${patsubst ${PROGRAM}/%, ${BUILD}/${PROGRAM}/%.o, ${SRCS}} ${STARTFILES}/start.o
DEPS=${patsubst ${PROGRAM}/%, ${BUILD}/${PROGRAM}/%.d, ${SRCS}}

CFLAGS=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL}
CFLAGS+=-DPLATFORM_${PLATFORM}
CFLAGS+=-nostdlib
CFLAGS+=-DSTACK_SIZE=1024
CFLAGS+=-Os -g3
CFLAGS+=-I${COMMON_INC} -include ${S3K_CONF_H}

LDFLAGS=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL}
LDFLAGS+=-g3
LDFLAGS+=-nostartfiles -nostdlib
LDFLAGS+=-T${PROGRAM}.ld -Tdefault.ld
LDFLAGS+=-Wl,--no-warn-rwx-segments
LDFLAGS+=-L${COMMON_LIB} -ls3k -laltc -lplat

ELF=${BUILD}/${PROGRAM}.elf

all: ${ELF}

clean:
	rm -f ${ELF} ${OBJS} ${DEPS}

${BUILD}/${PROGRAM}/%.S.o: ${PROGRAM}/%.S
	@mkdir -p ${@D}
	${CC} -o $@ $< ${CFLAGS} ${INC} -MMD -c

${BUILD}/${PROGRAM}/%.c.o: ${PROGRAM}/%.c
	@mkdir -p ${@D}
	${CC} -o $@ $< ${CFLAGS} ${INC} -MMD -c

${ELF}: ${OBJS}
	@mkdir -p ${@D}
	${CC} -o $@ ${OBJS} ${LDFLAGS} -MMD ${INC}

.PHONY: all elf clean

-include ${DEPS}

