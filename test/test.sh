#!/bin/sh

GDBARGS=
[ "${1}" = '-debug' ] && GDBARGS='gdb -ex r'

gcc -o test -g test.c -pthread && ${GDBARGS} ./test

