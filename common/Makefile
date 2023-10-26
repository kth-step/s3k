include plat/${PLATFORM}.mk
BUILD=build/${PLATFORM}

include ${ROOT}/tools.mk

CFLAGS=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL}
CFLAGS+=-DPLATFORM_${PLATFORM}
CFLAGS+=-Os
CFLAGS+=-flto -ffat-lto-objects
CFLAGS+=-nostdlib -Iinc

SRCS2OBJS=${patsubst src/%.S, ${BUILD}/%.o, ${filter %.S, ${1}}} \
	  ${patsubst src/%.c, ${BUILD}/%.o, ${filter %.c, ${1}}}

ALTC_SRCS=${wildcard src/altc/*.[cS]}
S3K_SRCS=${wildcard src/s3k/*.[cS]}
START_SRCS=${wildcard src/start/*.S}

PLAT_OBJS=${call SRCS2OBJS, ${PLAT_SRCS}}
ALTC_OBJS=${call SRCS2OBJS, ${ALTC_SRCS}}
S3K_OBJS=${call SRCS2OBJS, ${S3K_SRCS}}
START_OBJS=${call SRCS2OBJS, ${START_SRCS}}

all: ${BUILD}/libplat.a ${BUILD}/libaltc.a ${BUILD}/libs3k.a ${START_OBJS}

${BUILD}/libplat.a: ${PLAT_OBJS}
	echo ${PLAT_OBJS}
	@mkdir -p ${@D}
	${AR} cr $@ $^

${BUILD}/libaltc.a: ${ALTC_OBJS}
	@mkdir -p ${@D}
	${AR} cr $@ $^

${BUILD}/libs3k.a: ${S3K_OBJS}
	@mkdir -p ${@D}
	${AR} cr $@ $^

${BUILD}/%.o: src/%.c
	@mkdir -p ${@D}
	${CC} -o $@ $< -c ${CFLAGS}

${BUILD}/%.o: src/%.S
	@mkdir -p ${@D}
	${CC} -o $@ $< -c ${CFLAGS}
clean:
	rm -rf ${BUILD}

.PHONY: all clean
