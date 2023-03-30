RISCV_PREFIX?=riscv64-unknown-elf-
CC=$(RISCV_PREFIX)gcc
AR=$(RISCV_PREFIX)ar

CFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medany
CFLAGS+=-ffat-lto-objects -flto
CFLAGS+=-Os -g

all: libs3k.a

libs3k.a: s3k-syscall.o s3k-utils.o
	$(AR) rcs -o $@ $^

clean: 
	rm -f s3k-syscall.o s3k-utils.o

.PHONY: clean
