.POSIX:

ROOT:=${abspath ..}

include plat/${PLATFORM}.mk

include ${ROOT}/tools.mk

# CFLAGS
CFLAGS:=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL} \
	-DPLATFORM_${PLATFORM} \
	-Os -flto -ffat-lto-objects \
	-nostdlib -Iinc \

BUILD :=build/${PLATFORM}
SRCS2OBJS=${patsubst src/%.S, ${BUILD}/%.o, ${filter %.S, ${1}}} \
	  ${patsubst src/%.c, ${BUILD}/%.o, ${filter %.c, ${1}}}

ALTC_SRCS :=${wildcard src/altc/*.[cS]}
S3K_SRCS  :=${wildcard src/s3k/*.[cS]}
START_SRCS:=${wildcard src/start/*.S}

PLAT_OBJS :=${call SRCS2OBJS, ${PLAT_SRCS}}
ALTC_OBJS :=${call SRCS2OBJS, ${ALTC_SRCS}}
S3K_OBJS  :=${call SRCS2OBJS, ${S3K_SRCS}}
START_OBJS:=${call SRCS2OBJS, ${START_SRCS}}

TARGETS:=${BUILD}/libplat.a \
	 ${BUILD}/libaltc.a \
	 ${BUILD}/libs3k.a \
	 ${START_OBJS}


all: ${TARGETS}

clean:
	rm -rf ${BUILD}

${BUILD}/libplat.a: ${PLAT_OBJS}
	@mkdir -p ${@D}
	@${AR} cr $@ $^
	@printf "AR\t$@\n"

${BUILD}/libaltc.a: ${ALTC_OBJS}
	@mkdir -p ${@D}
	@${AR} cr $@ $^
	@printf "AR\t$@\n"

${BUILD}/libs3k.a: ${S3K_OBJS}
	@mkdir -p ${@D}
	@${AR} cr $@ $^
	@printf "AR\t$@\n"

${BUILD}/%.o: src/%.c
	@mkdir -p ${@D}
	@${CC} -o $@ $< -c ${CFLAGS}
	@printf "CC\t$@\n"

${BUILD}/%.o: src/%.S
	@mkdir -p ${@D}
	@${CC} -o $@ $< -c ${CFLAGS}
	@printf "CC\t$@\n"

.PHONY: all clean
