# See LICENSE file for copyright and license details.
.POSIX:

include config.mk

vpath %.c src
vpath %.S src

SRCS=head.S trap.S cap.c cnode.c current.c csr.c exception.c init.c \
     proc.c schedule.c syscall.c syscall_lock.c syscall_monitor.c \
     ticket_lock.c timer.c wfi.c kassert.c
OBJS=$(patsubst %, $(BUILD_DIR)/obj/%.o, ${SRCS})
DEPS=${OBJS:.o=.d}

all: options kernel dasm

options:
	@printf "build options:\n"
	@printf "CC        = ${CC}\n"
	@printf "LDFLAGS   = ${LDFLAGS}\n"
	@printf "ASFLAGS   = ${ASFLAGS}\n"
	@printf "CFLAGS    = ${CFLAGS}\n"
	@printf "INC       = ${INC}\n"
	@printf "CONFIG_H  = ${abspath ${CONFIG_H}}\n"
	@printf "BUILD_DIR = ${abspath ${BUILD_DIR}}\n"

kernel: $(BUILD_DIR)/s3k.elf 

dasm: $(BUILD_DIR)/s3k.da

test:
	$(MAKE) -C test

format:
	clang-format -i $(shell find -wholename "*.[chC]" -not -path '*/.*')

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR)/obj/%.S.o: %.S
	@mkdir -p ${@D}
	@printf "CC ${@F}\n"
	@${CC} ${ASFLAGS} ${INC} -MMD -c -o $@ $<

$(BUILD_DIR)/obj/%.c.o: %.c
	@mkdir -p ${@D}
	@printf "CC ${@F}\n"
	@${CC} ${CFLAGS} ${INC} -MMD -c -o $@ $<

$(BUILD_DIR)/s3k.elf: ${OBJS}
	@printf "CC ${@F}\n"
	@${CC} ${LDFLAGS} -o $@ $^

$(BUILD_DIR)/s3k.da: $(BUILD_DIR)/s3k.elf
	@printf "OBJDUMP ${<F} ${@F}$@\n"
	@${OBJDUMP} -d $< > $@

.PHONY: all options clean dasm docs test kernel

-include ${DEPS}
