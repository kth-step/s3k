#!/bin/sh

FILES=$(find -name *.[hc])
clang-format -i $FILES
