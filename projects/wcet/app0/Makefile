PROGRAM=app0
ELF=${PROGRAM}.elf
BIN=${PROGRAM}.bin
COMMON=../../../common
LIBS3K=../../../libs3k
DRIVERS=../../../drivers

PLATFORM=../../../plat/virt

include ${PLATFORM}/platform.mk

vpath %.c src

SRCS=${COMMON}/start.S ${COMMON}/altio.c main.c
SRCS+=${PLATFORM_SRCS}

CFLAGS+=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL}
CFLAGS+=-nostartfiles -nostdlib
CFLAGS+=-DSTACK_SIZE=1024
CFLAGS+=-T${PLATFORM}/platform.ld -Tlinker.ld
CFLAGS+=-Wl,--no-warn-rwx-segments

INC=-I${DRIVERS}/inc -I${COMMON}/inc -I${LIBS3K}

all: ${ELF} ${BIN} 

${ELF}: ${SRCS}
	$(CC) ${CFLAGS} ${INC} -o $@ ${filter %.S, $^} ${filter %.c, $^}

${BIN}: ${ELF}
	${OBJCOPY} -O binary $< $@

clean:
	rm -f ${ELF} ${BIN}

.PHONY: clean
