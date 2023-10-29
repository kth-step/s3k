.POSIX:
export ROOT=${abspath .}


PROJECTS=projects/hello projects/trapped projects/ping-pong
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

docs:
	doxygen

format:
	clang-format -i $(shell find -name '*.[hc]' -not -path '*/.*')

.PHONY: all docs clean common ${PROJECTS}
