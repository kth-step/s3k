.POSIX:
.SECONDARY:

BUILD   ?=build
PROGRAM ?=a

include ${ROOT}/tools.mk
include ${ROOT}/common/plat/${PLATFORM}.mk

C_SRCS:=${wildcard ${PROGRAM}/*.c}
S_SRCS:=${wildcard ${PROGRAM}/*.S}
OBJS  :=${patsubst %.c,${BUILD}/%.o,${C_SRCS}} \
	${patsubst %.S,${BUILD}/%.o,${S_SRCS}} \
	${STARTFILES}/start.o
DEPS  :=${OBJS:.o=.d}

CFLAGS:=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL} \
	-DPLATFORM_${PLATFORM} \
	-nostdlib \
	-Os -g3 -flto \
	-I${COMMON_INC} -include ${S3K_CONF_H}

LDFLAGS:=-march=${ARCH} -mabi=${ABI} -mcmodel=${CMODEL} \
	 -nostdlib \
	 -flto \
	 -T${PROGRAM}.ld -Tdefault.ld \
	 -Wl,--no-warn-rwx-segments \
	 -L${COMMON_LIB} \
	 -L/opt/homebrew/Cellar/riscv-gnu-toolchain/main/lib/gcc/riscv64-unknown-elf/12.2.0/rv32imac/ilp32 \
	 -ls3k -laltc -lplat -lgcc \

ELF:=${BUILD}/${PROGRAM}.elf
BIN:=${ELF:.elf=.bin}
HEX:=${ELF:.elf=.hex}
DA :=${ELF:.elf=.da}

all: ${ELF} ${BIN} ${HEX} ${DA}

clean:
	rm -f ${ELF} ${OBJS} ${DEPS}

${BUILD}/${PROGRAM}/%.o: ${PROGRAM}/%.S
	@mkdir -p ${@D}
	@${CC} -o $@ $< ${CFLAGS} ${INC} -MMD -c
	@printf "CC\t$@\n"

${BUILD}/${PROGRAM}/%.o: ${PROGRAM}/%.c
	@mkdir -p ${@D}
	@${CC} -o $@ $< ${CFLAGS} ${INC} -MMD -c
	@printf "CC\t$@\n"

%.elf: ${OBJS}
	@mkdir -p ${@D}
	@${CC} -o $@ ${OBJS} ${LDFLAGS} ${INC}
	@printf "CC\t$@\n"

%.bin: %.elf
	@${OBJCOPY} -O binary $< $@
	@printf "OBJCOPY\t$@\n"

%.hex: %.elf
	@${OBJCOPY} -O ihex $< $@
	@printf "OBJCOPY\t$@\n"

%.da: %.elf
	@${OBJDUMP} -D $< > $@
	@printf "OBJDUMP\t$@\n"

.PHONY: all clean

-include ${DEPS}
