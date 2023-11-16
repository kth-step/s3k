.POSIX:

export ROOT=${abspath .}


PROJECTS:=projects/hello \
	  projects/trapped \
	  projects/ping-pong \
	  projects/demonstrator
PLATFORM?=qemu_virt

include tools.mk
include common/plat/${PLATFORM}.mk

all: ${PROJECTS}

common ${PROJECTS}:
	make -C $@ all PLATFORM=${PLATFORM}

clean:
	for i in ${PROJECTS}; do \
		make -C $$i clean PLATFORM=${PLATFORM}; \
	done

format:
	clang-format -i $$(find * -type f -name '*.[hc]')

.PHONY: all docs clean common ${PROJECTS}
