
PLATFORM?=${patsubst plat/%.mk, %, ${wildcard plat/*.mk}}

all: ${PLATFORM}

${PLATFORM}:
	${MAKE} -f build.mk PLATFORM=$@ all

clean:
	for i in ${PLATFORM}; do \
		${MAKE} -f build.mk PLATFORM=$$i clean; \
	done

.PHONY: all
