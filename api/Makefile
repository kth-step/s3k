RISCV_PREFIX?=riscv64-unknown-elf-
CC=$(RISCV_PREFIX)gcc
AR=$(RISCV_PREFIX)ar

CFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medany
CFLAGS+=-ffat-lto-objects -flto
CFLAGS+=-Os -g

clean: s3k-syscall.o s3k-utils.o

libs3k.a: s3k-syscall.o s3k-utils.o
	$(AR) rcs -o $@ $^

.PHONY: clean
