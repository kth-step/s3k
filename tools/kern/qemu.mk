.SECONDARY:
.POSIX:

ifeq ($(S3K_ROOT),)
$(error "S3K_ROOT not set")
endif

VPATH=$(S3K_ROOT)/sys/src
INCLUDES=-I$(S3K_ROOT)/sys/includes
LINKER_SCRIPT=$(S3K_ROOT)/tools/kern/qemu.ld

CFLAGS=$(CONFIG) $(INCLUDES)
CFLAGS+=-march=rv64imac_zicsr -mabi=lp64 -mcmodel=medany
CFLAGS+=-nostdlib -nostartfiles
CFLAGS+=-O2 -g
CFLAGS+=-T$(LINKER_SCRIPT)
CFLAGS+=-include qemu.h

# Core kernel files
SRCS:=kern/trap.S kern/head.S
SRCS+=kern/proc.c kern/sched.c
# Exception handling
SRCS+=kern/interrupt.c kern/exception.c kern/syscall.c
# Capabilities
SRCS+=kern/cap.c
# ALT lib
SRCS+=alt/fprintf.c alt/printf.c alt/snprintf.c
# Platform specific files
SRCS+=kern/plat/qemu.c drivers/time.c drivers/uart/ns16550a.c

OBJS=$(patsubst %, objs/%.o, $(SRCS))
ELF=kernel.elf
HEX=kernel.hex


all: $(ELF) $(HEX)

clean:
	rm -f $(OBJS) $(ELF) $(HEX)

objs/%.c.o: %.c
	@mkdir -p $(@D)
	$(CC) -o $@ $< -c $(CFLAGS)

objs/%.S.o: %.S
	@mkdir -p $(@D)
	$(CC) -o $@ $< -c $(CFLAGS)

$(ELF): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@
