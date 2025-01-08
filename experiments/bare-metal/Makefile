CFLAGS      ?= -W -Wall -Wextra -Werror -Wundef -Wshadow -pedantic \
               -Wdouble-promotion -fno-common -Wconversion \
               -march=rv32imc_zicsr -mabi=ilp32 \
               -Os -ffunction-sections -fdata-sections \
               -I. -I$(MDK)/$(ARCH)

LINKFLAGS   ?= -Tmemory.ld -Tromfuncs.ld -Tcommon.ld -nostdlib -nostartfiles -Wl,--gc-sections $(EXTRA_LINKFLAGS)

program.elf: main.c
	riscv32-esp-elf-gcc  $(CFLAGS) start.S vectors.S syscalls.c main.c  $(LINKFLAGS) -o program.elf
program.bin: program.elf
	riscv32-esp-elf-objcopy -v -O binary program.elf program.bin
qemu_flash.bin: program.bin
	esptool.py --chip=esp32c3 merge_bin --output=qemu_flash.bin \
	--fill-flash-size=2MB --flash_mode dio --flash_freq 80m \
	--flash_size 2MB 0x0 program.bin
qemu: qemu_flash.bin
	qemu-system-riscv32 -M esp32c3 \
	-drive file=qemu_flash.bin,if=mtd,format=raw -nographic  -d guest_errors
debug: qemu_flash.bin
	qemu-system-riscv32 -M esp32c3 \
	-drive file=qemu_flash.bin,if=mtd,format=raw -nographic -s -S
