RISCV_PREFIX?=riscv64-unknown-elf-
CC=$(RISCV_PREFIX)gcc
AR=$(RISCV_PREFIX)ar

CFLAGS=-march=rv64imac -mabi=lp64 -mcmodel=medany
CFLAGS+=-ffat-lto-objects -flto 

libs3k.a: s3k-syscall.o s3k-utils.o
	$(AR) rcs -o $@ $^
