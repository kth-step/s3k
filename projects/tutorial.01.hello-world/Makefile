.POSIX:

export PLATFORM   ?=qemu_esp32c3
export ROOT       :=${abspath ../..}
export BUILD      :=${abspath build/${PLATFORM}}
export S3K_CONF_H :=${abspath s3k_conf.h}

include ${ROOT}/tools.mk

APPS=app0

ELFS:=${patsubst %, ${BUILD}/%.elf, kernel ${APPS}}

all: kernel ${APPS}

clean:
	rm -rf ${BUILD}

common:
	@${MAKE} -C ${ROOT}/common

kernel: common
	@${MAKE} -C ${ROOT}/kernel

${APPS}: common
	@${MAKE} -f ../build.mk PROGRAM=$@

qemu: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/qemu.sh

qemu-gdb: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/qemu.sh -gdb tcp::3333 -S

gdb: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/gdb.sh

gdb-openocd: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/gdb-openocd.sh

qemu_flash.bin: kernel
	esptool.py --chip=esp32c3 merge_bin --output=${BUILD}/qemu_flash.bin \
	--fill-flash-size=2MB --flash_mode dio --flash_freq 80m \
	--flash_size 2MB 0x0 ${BUILD}/kernel.bin

debug: qemu_flash.bin
	qemu-system-riscv32 -M esp32c3 \
	-drive file=${BUILD}/qemu_flash.bin,if=mtd,format=raw -nographic -s -S

size: ${ELFS}
	${SIZE} ${ELFS}

.PHONY: all clean size qemu qemu-gdb gdb kernel common ${APPS}
