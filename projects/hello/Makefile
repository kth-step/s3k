.POSIX:

export S3K_ROOT:=${abspath ../..}

include ${S3K_ROOT}/tools/tools.mk

PROGRAMS=kern
#app0 app1

ELFS:=${patsubst %, ${BUILD}/%.elf, kernel ${APPS}}

all: ${PROGRAMS}

clean:
	for i in $(PROGRAMS); do \
		${MAKE} -C $$i clean; \
	done

${PROGRAMS}:
	@${MAKE} -C $@ all

.PHONY: all clean ${PROGRAMS}
