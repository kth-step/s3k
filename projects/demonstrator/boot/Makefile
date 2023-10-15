PROGRAM=boot
BUILD?=build
SRCS=start/start.S s3klib/mem.S s3k/s3k.c s3klib/altio.c
SRCS+=main.c payload.S

include ../default.mk


# payload dependencies
${BUILD}/${PROGRAM}/payload.S.o: ../build/monitor.bin ../build/crypto.bin ../build/uart.bin


