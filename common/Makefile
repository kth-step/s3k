.POSIX:
.SECONDARY:

PLATFORM?=${patsubst plat/%.mk, %, ${wildcard plat/*.mk}}

all: ${addsuffix .all, ${PLATFORM}}
clean: ${addsuffix .clean, ${PLATFORM}}

%.all: 
	${MAKE} -f build.mk PLATFORM=${@:.all=} all

%.clean:
	${MAKE} -f build.mk PLATFORM=${@:.clean=} clean

.PHONY: all clean
