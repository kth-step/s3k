export ARCH=rv32imac_zicsr_zifencei
export ABI=ilp32
export CMODEL=medany
export COMMON_INC:=${ROOT}/common/inc
export COMMON_LIB:=${ROOT}/common/build/${PLATFORM}
export STARTFILES:=${ROOT}/common/build/${PLATFORM}/start
PLAT_SRCS=src/drivers/esp32c3.c
