# See LICENSE file for copyright and license details.
.POSIX:

include config.mk

vpath %.c src bsp/${PLATFORM}
vpath %.S src

CSRC=cap_node.c csr.c exception.c init.c lock.c proc.c sched.c syscall.c\
     timer.c init_caps.c
ASSRC=head.S trap.S stack.S
OBJ=${addprefix obj/, ${ASSRC:.S=.o} ${CSRC:.c=.o}}
DEP=${OBJ:.o=.d}

CONFIG_H?=config.h
DEFS=${addprefix -D,${S3K_CONF}}

all: options s3k.elf s3k.da

options:
	@printf "build options:\n"
	@printf "CC       = ${CC}\n"
	@printf "LDFLAGS  = ${LDFLAGS}\n"
	@printf "ASFLAGS  = ${ASFLAGS}\n"
	@printf "CFLAGS   = ${CFLAGS}\n"
	@printf "CONFIG_H = ${abspath CONFIG_H}\n"

obj:
	mkdir -p $@

inc/cap.h: cap.yml ./scripts/capgen.py
	./scripts/capgen.py $< $@


obj/%.o: %.S | obj
	${CC} ${ASFLAGS} -include ${CONFIG_H} ${INC} -MMD -c -o $@ $<

obj/%.o: %.c inc/cap.h | obj
	${CC} ${CFLAGS} -include ${CONFIG_H} ${INC} -MMD -c -o $@ $<

s3k.elf: ${OBJ}
	${CC} ${LDFLAGS} -o $@ ${OBJ}

s3k.da:	s3k.elf
	${OBJDUMP} -d $< > $@

clean:
	git clean -fdX

-include ${DEP}

.PHONY: all options clean
