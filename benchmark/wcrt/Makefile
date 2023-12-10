.POSIX:

export PLATFORM   ?=sifive_unleashed
export ROOT       :=${abspath ../..}
export BUILD      :=${abspath build/${PLATFORM}}
export S3K_CONF_H :=${abspath s3k_conf.h}

include ${ROOT}/tools.mk
include ${ROOT}/common/plat/${PLATFORM}.mk

APPS=app0 app1 app2

ELFS=${patsubst %, ${BUILD}/%.elf, kernel ${APPS}}

all: kernel ${APPS} size

clean:
	@${MAKE} -C ${ROOT}/common clean
	@${MAKE} -C ${ROOT}/kernel clean
	@for prog in ${APPS}; do \
		${MAKE} -f build.mk PROGRAM=$$prog clean; \
		done

common:
	@${MAKE} -C ${ROOT}/common

kernel: common
	@${MAKE} -C ${ROOT}/kernel

size: kernel ${APPS}
	${SIZE} ${BUILD}/*.elf

qemu: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/qemu.sh

qemu-gdb: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/qemu.sh -gdb tcp::3333 -S

gdb: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/gdb.sh

gdb-openocd: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/gdb-openocd.sh

${APPS}: common
	@${MAKE} -f build.mk PROGRAM=$@

.PHONY: all clean size qemu qemu-gdb gdb kernel common ${APPS}
