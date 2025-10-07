#!/bin/sh

docker run -it --rm -v .:/workspace -w /workspace hakarlsson/riscv-picolibc
