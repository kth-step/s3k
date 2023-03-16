# See LICENSE file for copyright and license details.
.POSIX:

include config.mk

vpath %.c src
vpath %.S src

ASSRC=head.S trap.S stack.S
CSRC=cap.c cnode.c current.c csr.c exception.c init.c lock.c proc.c schedule.c \
     syscall.c syscall_monitor.c timer.c wfi.c
OBJ=${addprefix obj/, ${ASSRC:.S=.o} ${CSRC:.c=.o}}
DEP=${OBJ:.o=.d}

CONFIG_H?=config.h
PLATFORM_H?=plat/${PLATFORM}/platform.h
INC+=-include ${CONFIG_H} -Iplat/${PLATFORM}

all: options test kernel dasm

options:
	@printf "build options:\n"
	@printf "CC       = ${CC}\n"
	@printf "LDFLAGS  = ${LDFLAGS}\n"
	@printf "ASFLAGS  = ${ASFLAGS}\n"
	@printf "CFLAGS   = ${CFLAGS}\n"
	@printf "CONFIG_H = ${abspath ${CONFIG_H}}\n"

kernel: s3k.elf 
dasm: s3k.da

docs:
	doxygen

test:
	$(MAKE) -C test

format:
	clang-format --dry-run --Werror $(shell find -regextype sed -regex ".*\.\(c\|cc\|h\)" -not -path '*/.*')

clean:
	git clean -fdX

obj/%.o: %.S
	@mkdir -p ${@D}
	${CC} ${ASFLAGS} ${INC} -MMD -c -o $@ $<

obj/%.o: %.c
	@mkdir -p ${@D}
	${CC} ${CFLAGS} ${INC}  -MMD -c -o $@ $<

s3k.elf: ${OBJ}
	${CC} ${LDFLAGS} -o $@ ${OBJ}

s3k.da:	s3k.elf
	${OBJDUMP} -d $< > $@

-include ${DEP}

.PHONY: all options clean dasm docs test kernel
