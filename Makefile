.POSIX:
.SECONDARY:

PROJECTS:=projects/hello \
	  projects/trapped \
	  projects/ping-pong

all: ${PROJECTS}

${PROJECTS}: common

common ${PROJECTS}:
	@${MAKE} -C $@ all

clean:
	@for i in common ${PROJECTS}; do \
		${MAKE} -C $$i clean; \
	done

format:
	clang-format -i $$(find * -type f -name '*.[hc]')

.PHONY: all clean format common ${PROJECTS}
