
include ../tools.mk
include plat/${PLATFORM}.mk

BUILD=plat/${PLATFORM}

CFLAGS=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL}
CFLAGS+=-DPLATFORM_${PLATFORM}
CFLAGS+=-Os
CFLAGS+=-flto -ffat-lto-objects
CFLAGS+=-nostdlib -Iinc

ALTC_SRCS=${wildcard src/altc/*.[cS]}
S3K_SRCS=${wildcard src/s3k/*.[cS]}
START_SRCS=${wildcard src/start/*.S}

PLAT_OBJS=${patsubst src/%, ${BUILD}/obj/%.o, ${PLAT_SRCS}}
ALTC_OBJS=${patsubst src/%, ${BUILD}/obj/%.o, ${ALTC_SRCS}}
S3K_OBJS=${patsubst src/%, ${BUILD}/obj/%.o, ${S3K_SRCS}}
START_OBJS=${patsubst src/start/%.S, ${BUILD}/start/%.o, ${START_SRCS}}

all: ${BUILD}/libplat.a ${BUILD}/libaltc.a ${BUILD}/libs3k.a ${START_OBJS}

${BUILD}/libplat.a: ${PLAT_OBJS} 
	@mkdir -p ${@D}
	${AR} cr -flto $@ $^

${BUILD}/libaltc.a: ${ALTC_OBJS} 
	@mkdir -p ${@D}
	${AR} cr $@ $^

${BUILD}/libs3k.a: ${S3K_OBJS} 
	@mkdir -p ${@D}
	${AR} cr $@ $^

${BUILD}/obj/%.c.o: src/%.c
	@mkdir -p ${@D}
	${CC} -o $@ $< -c ${CFLAGS}

${BUILD}/obj/%.S.o: src/%.S
	@mkdir -p ${@D}
	${CC} -o $@ $< -c ${CFLAGS}
${BUILD}/start/%.o: src/start/%.S
	@mkdir -p ${@D}
	${CC} -o $@ $< -c ${CFLAGS}
clean:
	rm -rf ${BUILD}

.PHONY: all clean
