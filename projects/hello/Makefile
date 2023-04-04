export BUILD=${abspath build}

DIRS=../../kernel app0 app1

all: ${DIRS}

${DIRS}:
	@${MAKE} -C $@

clean:
	rm -rf ${BUILD}

.PHONY: all clean ${DIRS}
