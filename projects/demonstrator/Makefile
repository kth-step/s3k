BUILD      ?=build
S3K_CONF_H ?=s3k_conf.h

SUBDIRS=../../kernel boot monitor uartppp crypto app0 app1

all: ${SUBDIRS}

boot: monitor uartppp crypto

${SUBDIRS}:
	@${MAKE} -C $@ all \
		BUILD=${abspath ${BUILD}} \
		S3K_CONF_H=${abspath ${S3K_CONF_H}}

qemu gdb: all
	./scripts/$@.sh

clean:
	rm -rf ${BUILD}

.PHONY: all clean ${SUBDIRS} qemu gdb
