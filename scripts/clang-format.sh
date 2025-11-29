#!/usr/bin/env bash

clang-format -i $(find ./ -name '*.c' -o -name '*.h')
