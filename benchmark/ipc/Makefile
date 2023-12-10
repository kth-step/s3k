.POSIX:

export PLATFORM   ?=sifive_unleashed
export ROOT       :=${abspath ../..}
export BUILD      :=${abspath build/${PLATFORM}}
export S3K_CONF_H :=${abspath s3k_conf.h}

APPS=app0 app1

ELFS:=${patsubst %, ${BUILD}/%.elf, kernel ${APPS}}

all: kernel ${APPS}

clean:
	rm -rf ${BUILD}

common:
	@${MAKE} -C ${ROOT}/common

kernel: common
	@${MAKE} -C ${ROOT}/kernel

${APPS}: common
	@${MAKE} -f build.mk PROGRAM=$@

qemu: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/qemu.sh

qemu-gdb: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/qemu.sh -gdb tcp::3333 -S

gdb: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/gdb.sh

gdb-openocd: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/gdb-openocd.sh

.PHONY: all clean qemu qemu-gdb gdb kernel common ${APPS}
