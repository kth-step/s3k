export BUILD=${abspath build}
export S3K_CONF_H=${abspath s3k_conf.h}

DIRS=../../kernel app0 app1

all: ${DIRS}

${DIRS}:
	@${MAKE} -C $@

gdb qemu: all
	./scripts/$@.sh

clean:
	rm -rf ${BUILD}

.PHONY: all clean ${DIRS} gdb qemu
