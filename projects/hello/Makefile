export PLATFORM  ?=qemu_virt
export ROOT      :=${abspath ../..}
export BUILD     :=${abspath build}
export S3K_CONF_H:=${abspath s3k_conf.h}

include ${ROOT}/common/plat/${PLATFORM}.mk

APPS=app0 app1

ELFS=${patsubst %, ${BUILD}/%.elf, kernel ${APPS}}

all: kernel ${APPS}

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

qemu: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/qemu.sh

qemu-gdb: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/qemu.sh -gdb tcp::3333 -S

gdb: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/gdb.sh

${APPS}: common
	@${MAKE} -f build.mk PROGRAM=$@

.PHONY: all clean qemu qemu-gdb gdb kernel common ${APPS}
