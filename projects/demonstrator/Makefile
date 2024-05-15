.POSIX:

export PLATFORM   ?=qemu_virt
export ROOT       :=${abspath ../..}
export BUILD      :=${abspath build/${PLATFORM}}
export S3K_CONF_H :=${abspath s3k_conf.h}

include ${ROOT}/common/plat/${PLATFORM}.mk

APPS=boot app0 app1 monitor crypto uartppp

ELFS:=${patsubst %,${BUILD}/%.elf,kernel boot}

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

boot: monitor crypto uartppp
${APPS}:
	@${MAKE} -f build.mk PROGRAM=$@

qemu: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/qemu.sh -serial tcp:localhost:8888,server,nowait

qemu-gdb: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/qemu.sh -serial tcp:localhost:8888,server,nowait -gdb tcp::3333 -S

gdb: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/gdb.sh

gdb-openocd: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/gdb-openocd.sh



.PHONY: all clean qemu qemu-gdb gdb kernel common ${APPS}
