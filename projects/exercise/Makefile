.POSIX:

export PLATFORM   ?=qemu_virt4
export ROOT       :=${abspath ../..}
export BUILD      :=${abspath build/${PLATFORM}}
export S3K_CONF_H :=${abspath s3k_conf.h}

include ${ROOT}/tools.mk

APPS=app0 app1 app2

ELFS:=${patsubst %, ${BUILD}/%.elf, kernel ${APPS}}
INIT_ELFS:=${patsubst %, ${BUILD}/%.elf, kernel app0 app1}

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
	@ELFS="${INIT_ELFS}" ${ROOT}/scripts/qemu.sh -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

qemu-gdb: kernel ${APPS}
	@ELFS="${ELFS}" ${ROOT}/scripts/qemu.sh -gdb tcp::3333 -S -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

gdb: kernel ${APPS} disk-image
	@ELFS="${ELFS}" ${ROOT}/scripts/gdb.sh

gdb-openocd: kernel ${APPS}
	@ELFS="${INIT_ELFS}" ${ROOT}/scripts/gdb-openocd.sh

disk-image: ${APPS}
	qemu-img create fs.img 10M
	mformat -i fs.img ::
	mcopy -i fs.img build/qemu_virt4/app2.bin ::/app2.bin

disk-read:
	mdir -i fs.img
	mcopy -i fs.img ::/NEWFILE.TXT extracted.txt
	cat < extracted.txt
	rm extracted.txt

size: ${ELFS}
	${SIZE} ${ELFS}

.PHONY: all clean size qemu qemu-gdb gdb kernel common ${APPS}
