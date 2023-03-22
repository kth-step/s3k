# See LICENSE file for copyright and license details.
.POSIX:

include config.mk

vpath %.c src
vpath %.S src

SRCS=head.S trap.S stack.S cap.c cnode.c current.c csr.c exception.c init.c \
     lock.c proc.c schedule.c syscall.c syscall_monitor.c timer.c wfi.c
OBJS=$(patsubst %, $(BUILD)/%.o, ${SRCS})
DEPS=${OBJS:.o=.d}

all: options kernel dasm

options:
	@printf "build options:\n"
	@printf "CC       = ${CC}\n"
	@printf "LDFLAGS  = ${LDFLAGS}\n"
	@printf "ASFLAGS  = ${ASFLAGS}\n"
	@printf "CFLAGS   = ${CFLAGS}\n"
	@printf "INC      = ${INC}\n"
	@printf "CONFIG_H = ${abspath ${CONFIG_H}}\n"

kernel: s3k.elf 

dasm: s3k.da

docs:
	doxygen

test:
	$(MAKE) -C test

format:
	clang-format -i $(shell find -regextype sed -regex ".*\.\(c\|cc\|h\)" -not -path '*/.*')

clean:
	git clean -fdX

$(BUILD)/%.S.o: %.S
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@${CC} ${ASFLAGS} ${INC} -MMD -c -o $@ $<

$(BUILD)/%.c.o: %.c
	@mkdir -p ${@D}
	@printf "CC $@\n"
	@${CC} ${CFLAGS} ${INC} -MMD -c -o $@ $<

%.elf: ${OBJS}
	@printf "CC $@\n"
	@${CC} ${LDFLAGS} -o $@ $^

%.da: %.elf
	@printf "OBJDUMP $< $@\n"
	@${OBJDUMP} -d $< > $@

.PHONY: all options clean dasm docs test kernel

-include ${DEPS}
