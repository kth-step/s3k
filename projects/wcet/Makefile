
export CC=riscv64-unknown-elf-gcc
export OBJCOPY=riscv64-unknown-elf-objcopy

all: s3k app0

s3k: 
	${MAKE} -C ../../kernel BUILD=${abspath s3k}

app0:
	${MAKE} -C app0

clean:
	rm -rf s3k
	${MAKE} -C app0 clean

.PHONY: clean app0 s3k
