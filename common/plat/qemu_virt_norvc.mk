export ARCH=rv64ima_zicsr_zifencei
export ABI=lp64
export CMODEL=medany
export COMMON_INC:=${ROOT}/common/inc
export COMMON_LIB:=${ROOT}/common/build/${PLATFORM}
export STARTFILES:=${ROOT}/common/build/${PLATFORM}/start
PLAT_SRCS=src/drivers/uart/ns16550a.c src/drivers/time.c
